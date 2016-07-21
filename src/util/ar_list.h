#define AR_LIST_DECLARATION(list_name, list_value_type) \
\
struct list_name##_node {\
    list_value_type value;\
    struct list_name##_node *next;\
    struct list_name##_node *prev;\
};\
\
struct list_name {\
    int n;\
    struct list_name##_node *head;\
    struct list_name##_node *tail;\
};\
\
void list_name##_init(struct list_name *list);\
int list_name##_empty(struct list_name *list);\
void list_name##_node_link(struct list_name##_node *a, struct list_name##_node *b);\
void list_name##_add_tail(struct list_name *list, list_value_type value);\
void list_name##_add_head(struct list_name *list, list_value_type value);\
list_value_type list_name##_pop_tail(struct list_name *list);\
list_value_type list_name##_pop_head(struct list_name *list);\
void list_name##_free(struct list_name *list);

#define AR_LIST_IMPLEMENTATION(list_name, list_value_type) \
\
void list_name##_init(struct list_name *list){\
    list->n = 0;\
    list->head = NULL;\
    list->tail = NULL;\
}\
\
int list_name##_empty(struct list_name *list){\
    return list->head == NULL;\
}\
\
void list_name##_node_link(struct list_name##_node *a, struct list_name##_node *b){\
    a->next = b;\
    b->prev = a;\
}\
\
void list_name##_add_tail(struct list_name *list, list_value_type value){\
    struct list_name##_node *node = malloc(sizeof(*node));\
    node->value = value;\
    node->next = NULL;\
\
    if (list->head == NULL){\
        node->prev = NULL;\
        list->head = node;\
    }else{\
        list_name##_node_link(list->tail, node);\
    }\
\
    list->tail = node;\
    list->n++;\
}\
\
void list_name##_add_head(struct list_name *list, list_value_type value){\
    struct list_name##_node *node = malloc(sizeof(*node));\
    node->value = value;\
    node->prev = NULL;\
\
    if (list->head == NULL){\
        node->next = NULL;\
        list->tail = node;\
    }else{\
        list_name##_node_link(node, list->head);\
    }\
\
    list->head = node;\
    list->n++;\
}\
\
list_value_type list_name##_pop_tail(struct list_name *list){\
    struct list_name##_node *node = list->tail;\
    struct list_name##_node *prev = node->prev;\
    list_value_type value;\
    value = node->value;\
\
    if (prev == NULL){\
        list->head = NULL;\
        list->tail = NULL;\
    }else{\
        prev->next = NULL;\
        list->tail = prev;\
    }\
\
    free(node);\
\
    list->n--;\
\
    return value;\
}\
\
list_value_type list_name##_pop_head(struct list_name *list){\
    struct list_name##_node *node = list->head;\
    struct list_name##_node *next = node->next;\
    list_value_type value;\
    value = node->value;\
\
    if (next == NULL){\
        list->head = NULL;\
        list->tail = NULL;\
    }else{\
        next->prev = NULL;\
        list->head = next;\
    }\
\
    free(node);\
\
    list->n--;\
\
    return value;\
}\
\
void list_name##_free(struct list_name *list){\
    while (!list_name##_empty(list)){\
        list_name##_pop_tail(list);\
    }\
}

