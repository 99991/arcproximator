/* TODO split into *.c and *.h */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

int svg_is_digit(char c){
    return c >= '0' && c <= '9';
}

uint32_t svg_parse_u32(char *c, char **end){
    uint32_t x = 0, base = 10;
    for (; svg_is_digit(*c); c++) x = x*base + (*c - '0');
    if (end) *end = c;
    return x;
}

double svg_pow10(uint32_t n){
    if (n < 10){
        double powers[10] = {1, 10, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9};
        return powers[n];
    }else{
        double x = svg_pow10(n >> 1);
        return n&1 ? 10.0*x*x : x*x;
    }
}

/* Some versions of msvcrt.dll have a O(strlen(c)) implementation of strtod. */
/* Some other versions are just slow. */
double svg_fast_strtod(char *c, char **end){
    int neg = 0;

    if (*c == '+') c++;
    if (*c == '-'){
        neg = 1;
        c++;
    }

    double a = svg_parse_u32(c, &c);

    if (*c == '.'){
        c++;
        char *start = c;
        double b = svg_parse_u32(c, &c);
        a += b/svg_pow10(c - start);
    }

    if (*c == 'e' || *c == 'E' || *c == 'd' || *c == 'D'){
        c++;

        int exp_neg = 0;

        if (*c == '+') c++;
        if (*c == '-'){
            exp_neg = 1;
            c++;
        }

        double exponent = svg_pow10(svg_parse_u32(c, &c));
        if (exp_neg) a /= exponent;
        else         a *= exponent;
    }

    if (end) *end = c;

    return neg ? -a : a;
}

struct svg_parser {
    uint8_t *ptr, *end;
    char *name;
    size_t name_len;
    char *value;
    size_t value_len;
};

int svg_is_space(uint8_t x){
    return x == ' ' || x == '\t' || x == '\r' || x == '\n';
}

int svg_is_name(uint8_t x){
    return
        /* skipping utf8 */
        (x >= 0x80) ||
        (x >= 'a' && x <= 'z') ||
        (x >= 'A' && x <= 'Z') ||
        (x >= '0' && x <= '9') ||
        (x == '_') || (x == '-') || (x == '.') || (x == ':');
}

void svg_skip(struct svg_parser *p, uint8_t x){
    if (p->ptr < p->end && *p->ptr != x){
        printf("ERROR: expected %c\n", x);
    }
    assert(p->ptr < p->end && *p->ptr == x);
    p->ptr++;
}

void svg_skip_text(struct svg_parser *p, const char *text){
    while (*text) svg_skip(p, *text++);
}

void skip_space(char **ptrptr, char *end){
    char *ptr = *ptrptr;
    while (ptr < end && svg_is_space(*ptr)) ptr++;
    *ptrptr = ptr;
}

void svg_skip_space(struct svg_parser *p){
    uint8_t *ptr = p->ptr;
    while (ptr < p->end && svg_is_space(*ptr)) ptr++;
    p->ptr = ptr;
}

void svg_parse_content(struct svg_parser *p){
    uint8_t *ptr = p->ptr;
    while (ptr < p->end && *ptr != '<') ptr++;
    p->ptr = ptr;
}

void svg_parse_name(struct svg_parser *p){
    uint8_t *ptr = p->ptr;
    p->name = (char*)ptr;
    while (ptr < p->end && svg_is_name(*ptr)) ptr++;
    p->name_len = ptr - (uint8_t*)p->name;
    p->ptr = ptr;
}

void svg_parse_string(struct svg_parser *p){
    svg_skip(p, '"');
    uint8_t *ptr = p->ptr;
    p->value = (char*)ptr;
    while (ptr < p->end && *ptr != '"'){
        if (*ptr++ == '\\'){
            /* skip escaped character */
            ptr++;
        }
    }
    p->value_len = ptr - (uint8_t*)p->value;
    p->ptr = ptr;
    svg_skip(p, '"');
}

int svg_is_numeric(char c){
    return c == '.' || c == '-' || c == '+' || (c >= '0' && c <= '9');
}

void svg_parse_numbers(char *ptr, char *end, char **end_ptr, double *numbers, size_t n){
    size_t i;
    for (i = 0; i < n; i++){
        assert(ptr < end);
        char *start = ptr;
        numbers[i] = svg_fast_strtod(ptr, &ptr);
        assert(start != ptr);
        skip_space(&ptr, end);
        if (ptr < end && *ptr == ',') ptr++;
        skip_space(&ptr, end);
    }
    if (end_ptr) *end_ptr = ptr;
}

void svg_parse_path(char *ptr, char *end){
    double numbers[8];
    double x0 = 0.0;
    double y0 = 0.0;
    double x1 = 0.0;
    double y1 = 0.0;
    double x2 = 0.0;
    double y2 = 0.0;
    double x3 = 0.0;
    double y3 = 0.0;
    double initial_x = 0.0;
    double initial_y = 0.0;

    char prev_command = 'z';
    while (ptr < end){
        skip_space(&ptr, end);

        if (ptr >= end) break;

        char command = *ptr++;

        skip_space(&ptr, end);

        switch (command){

        case 'm':
            svg_parse_numbers(ptr, end, &ptr, numbers, 2);
            x0 += numbers[0];
            y0 += numbers[1];
            if (prev_command == 'z' || prev_command == 'Z'){
                initial_x = x0;
                initial_y = y0;
            }
        case 'l':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 2);
                x1 = x0 + numbers[0];
                y1 = y0 + numbers[1];
                draw_line(x0, y0, x1, y1);
                x0 = x1;
                y0 = y1;
            }
            break;

        case 'M':
            svg_parse_numbers(ptr, end, &ptr, numbers, 2);
            x0 = numbers[0];
            y0 = numbers[1];
            if (prev_command == 'z' || prev_command == 'Z'){
                initial_x = x0;
                initial_y = y0;
            }
        case 'L':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 2);
                x1 = numbers[0];
                y1 = numbers[1];
                draw_line(x0, y0, x1, y1);
                x0 = x1;
                y0 = y1;
            }
            break;

        case 'v':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 1);
                y1 = y0 + numbers[0];
                draw_line(x0, y0, x0, y1);
                y0 = y1;
            }
            break;

        case 'V':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 1);
                y1 = numbers[0];
                draw_line(x0, y0, x0, y1);
                y0 = y1;
            }
            break;

        case 'h':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 1);
                x1 = x0 + numbers[0];
                draw_line(x0, y0, x1, y0);
                x0 = x1;
            }
            break;

        case 'H':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 1);
                x1 = numbers[0];
                draw_line(x0, y0, x1, y0);
                x0 = x1;
            }
            break;

        case 'q':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 4);
                x1 = x0 + numbers[0];
                y1 = y0 + numbers[1];
                x2 = x0 + numbers[2];
                y2 = y0 + numbers[3];
                draw_quadratic(x0, y0, x1, y1, x2, y2);
                x0 = x2;
                y0 = y2;
            }
            break;

        case 'Q':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 4);
                x1 = numbers[0];
                y1 = numbers[1];
                x2 = numbers[2];
                y2 = numbers[3];
                draw_quadratic(x0, y0, x1, y1, x2, y2);
                x0 = x2;
                y0 = y2;
            }
            break;

        case 'c':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 6);
                x1 = x0 + numbers[0];
                y1 = y0 + numbers[1];
                x2 = x0 + numbers[2];
                y2 = y0 + numbers[3];
                x3 = x0 + numbers[4];
                y3 = y0 + numbers[5];
                draw_cubic(x0, y0, x1, y1, x2, y2, x3, y3);
                x0 = x3;
                y0 = y3;
            }
            break;

        case 'C':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 6);
                x1 = numbers[0];
                y1 = numbers[1];
                x2 = numbers[2];
                y2 = numbers[3];
                x3 = numbers[4];
                y3 = numbers[5];
                draw_cubic(x0, y0, x1, y1, x2, y2, x3, y3);
                x0 = x3;
                y0 = y3;
            }
            break;

        case 't':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 2);
                if (prev_command == 'q' || prev_command == 'Q'){
                    x1 = x0 + (x0 - x1);
                    y1 = y0 + (y0 - y1);
                }else{
                    x1 = numbers[0];
                    y1 = numbers[1];
                }
                x2 = x0 + numbers[0];
                y2 = y0 + numbers[1];
                draw_quadratic(x0, y0, x1, y1, x2, y2);
                x0 = x2;
                y0 = y2;
            }
            break;

        case 'T':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 2);
                if (prev_command == 'q' || prev_command == 'Q'){
                    x1 = x0 + (x0 - x1);
                    y1 = y0 + (y0 - y1);
                }else{
                    x1 = numbers[0];
                    y1 = numbers[1];
                }
                x2 = numbers[0];
                y2 = numbers[1];
                draw_quadratic(x0, y0, x1, y1, x2, y2);
                x0 = x2;
                y0 = y2;
            }
            break;

        case 's':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 4);
                if (prev_command == 'c' || prev_command == 'C'){
                    x1 = x0 + (x0 - x2);
                    y1 = y0 + (y0 - y2);
                }else{
                    x1 = numbers[0];
                    y1 = numbers[1];
                }
                x2 = x0 + numbers[0];
                y2 = y0 + numbers[1];
                x3 = x0 + numbers[2];
                y3 = y0 + numbers[3];
                draw_cubic(x0, y0, x1, y1, x2, y2, x3, y3);
                x0 = x3;
                y0 = y3;
            }
            break;

        case 'S':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 4);
                if (prev_command == 'c' || prev_command == 'C'){
                    x1 = x0 + (x0 - x2);
                    y1 = y0 + (y0 - y2);
                }else{
                    x1 = numbers[0];
                    y1 = numbers[1];
                }
                x2 = numbers[0];
                y2 = numbers[1];
                x3 = numbers[2];
                y3 = numbers[3];
                draw_cubic(x0, y0, x1, y1, x2, y2, x3, y3);
                x0 = x3;
                y0 = y3;
            }
            break;

        case 'a':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 7);
                /* TODO handle properly */
                x1 = x0 + numbers[5];
                y1 = y0 + numbers[6];
                draw_line(x0, y0, x1, y1);
                x0 = x1,
                y0 = y1;
            }
            break;

        case 'A':
            while (ptr < end && svg_is_numeric(*ptr)){
                svg_parse_numbers(ptr, end, &ptr, numbers, 7);
                /* TODO handle properly */
                x1 = numbers[5];
                y1 = numbers[6];
                draw_line(x0, y0, x1, y1);
                x0 = x1,
                y0 = y1;
            }
            break;

        case 'z':
        case 'Z':
            x1 = initial_x;
            y1 = initial_y;
            draw_line(x0, y0, x1, y1);
            x0 = x1;
            y0 = y1;
            break;

        default:
            printf("TODO: unhandled command '%c'\n", command);
            exit(-1);
        }

        prev_command = command;
    }
}

void svg_parse_attribute(struct svg_parser *p){
    svg_parse_name(p);
    svg_skip(p, '=');
    svg_parse_string(p);

    if (p->name_len == 1 && memcmp(p->name, "d", p->name_len) == 0){
        //printf("%.*s = %.*s\n", p->name_len, p->name, p->value_len, p->value);

        svg_parse_path(p->value, p->value + p->value_len);
    }
}

void svg_parse_attributes(struct svg_parser *p){
    svg_skip_space(p);
    while (p->ptr < p->end && svg_is_name(*p->ptr)){
        svg_parse_attribute(p);
        svg_skip_space(p);
    }
}

void svg_parse_tag(struct svg_parser *p);

void svg_parse_element(struct svg_parser *p){
    svg_skip(p, '<');
    svg_parse_name(p);
    svg_skip_space(p);
    svg_parse_attributes(p);
    if (p->ptr < p->end && *p->ptr == '/'){
        svg_skip_text(p, "/>");
        return;
    }
    svg_skip(p, '>');

    svg_parse_content(p);
    while (p->ptr < p->end){
        if (*p->ptr == '<'){
            if (p->ptr + 1 < p->end && p->ptr[1] == '/') break;
            svg_parse_tag(p);
        }else{
            svg_parse_content(p);
        }
    }

    svg_skip_text(p, "</");
    svg_parse_name(p);
    svg_skip_space(p);
    svg_skip(p, '>');
}

void svg_parse_processing_instruction(struct svg_parser *p){
    svg_skip_text(p, "<?");
    svg_parse_name(p);
    svg_parse_attributes(p);
    svg_skip_text(p, "?>");
}

void svg_parse_between(struct svg_parser *p, const char *pre, const char *post){
    svg_skip_text(p, pre);
    uint8_t *ptr = p->ptr;
    size_t n = strlen(post);
    while (ptr + n <= p->end && memcmp(ptr, post, n) != 0) ptr++;
    p->ptr = ptr;
    svg_skip_text(p, post);
}

void svg_parse_array(struct svg_parser *p){
    svg_skip(p, '[');
    svg_skip_space(p);
    while (p->ptr < p->end && *p->ptr != ']'){
        svg_parse_tag(p);
        svg_skip_space(p);
    }
    svg_skip(p, ']');
}

void svg_parse_declaration(struct svg_parser *p, const char *which){
    svg_skip_text(p, "<!");
    svg_skip_text(p, which);
    svg_skip_space(p);

    while (p->ptr < p->end){
        uint8_t a = *p->ptr;
        if (a == '>') break;
        else if (svg_is_name(a)) svg_parse_name(p);
        else if (a == '[') svg_parse_array(p);
        else if (a == '"') svg_parse_string(p);
        else{
            fprintf(stderr, "Unexpected symbol");
            exit(-1);
        }
        svg_skip_space(p);
    }
    svg_skip(p, '>');
}

void svg_parse_tag(struct svg_parser *p){
    assert(*p->ptr == '<');
    assert(p->ptr + 3 <= p->end);
    unsigned char b = p->ptr[1];
    unsigned char c = p->ptr[2];
    if (svg_is_name(b)) svg_parse_element(p);
    else if (b == '?') svg_parse_processing_instruction(p);
    else if (b == '!' && c == '[') svg_parse_between(p, "<![CDATA[", "]]>");
    else if (b == '!' && c == '-') svg_parse_between(p, "<!--", "-->");
    else if (b == '!' && c == 'D') svg_parse_declaration(p, "DOCTYPE");
    else if (b == '!' && c == 'E') svg_parse_declaration(p, "ENTITY");
    else{
        fprintf(stderr, "Could not recognize tag\n");
        exit(-1);
    }
}

void svg_parse(struct svg_parser *p, void *bytes, size_t n_bytes){
    p->ptr = bytes;
    p->end = p->ptr + n_bytes;

    svg_skip_space(p);
    while (p->ptr < p->end){
        svg_parse_tag(p);
        svg_skip_space(p);
    }
}

void svg_parse_file(const char *path){
    struct svg_parser parser[1];
    size_t n_bytes;
    void *bytes = ar_load_file(path, &n_bytes);
    assert(bytes);
    svg_parse(parser, bytes, n_bytes);
    free(bytes);
}
