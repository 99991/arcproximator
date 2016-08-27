from common import *

def draw_segments(segments):
    random.seed(1)
    for i, segment in enumerate(segments):
        color = colors[i % len(colors)]
        write(str(i), segment.lerp(0.3), color)
        draw_line(segment[0], segment[1], color)
    
def segments_from_points(points):
    segments = []
    a = points[-1]
    for b in points:
        segments.append(Segment(a, b))
        a = b
    return segments

def handle_self_intersection(segments):
    split_points = [[] for _ in range(len(segments))]

    all_intersections = [Point(0, 0)]
    for i in range(len(segments)):
        seg_ab = segments[i]
        for j in range(i+1, len(segments)):
            seg_cd = segments[j]
            
            intersections = intersect(seg_ab, seg_cd)
            
            split_points[i].extend(intersections)
            split_points[j].extend(intersections)

    return split_things(segments, split_points, split_segment)

def sweep(segments):
    new_segments = []
    split_points = [[] for _ in range(len(segments))]
    
    vertices = list(sorted(set(a for a, b in segments), key=second))
    vertices_with_same_y = itertools.groupby(vertices, second)

    if 0:
        draw_segments(segments)
    
    for y, group in vertices_with_same_y:
        intersections = []

        for i, seg_ab in enumerate(segments):
            a, b = seg_ab
            
            # ignore horizontal segments
            if a.y == b.y:
                continue
            
            for intersection in sweep_intersect(y, seg_ab):
                # segments are half-open
                # so we ignore the upper segment point
                if intersection.y == max(a.y, b.y):
                    continue

                intersections.append((intersection, i))

        intersections.sort(key=first)
        group = sorted(group, key=first)

        if 0:
            next_color = itertools.cycle(colors)
            for s, i in intersections:
                print(s)
                write(str(i), s)
            print("")

        for p, q in zip(intersections[0::2], intersections[1::2]):
            a, i = p
            b, j = q
            between = [p for p in group if a.x <= p.x <= b.x]
            if between and a != b:
                split_points[i].append(a)
                split_points[j].append(b)
                points = [a] + between + [b]
                for a, b in zip(points, points[1:]):
                    ab = Segment(a, b)
                    ba = Segment(b, a)
                    if ab not in segments and ba not in segments:
                        new_segments.append(ab)
                        new_segments.append(ba)

                        if 0:
                            color = next(next_color)
                            draw_line(a, b, color=color)
                            draw_circle(a, 5, color=color)
                            draw_circle(b, 10, color=color)
                    a = b

    segments = split_things(segments, split_points, split_segment)
    
    segments.extend(new_segments)
    
    return segments

def remove_zero_length_segments(segments):
    return [segment for segment in segments if segment[0].dist2(segment[1]) > 0]

def eat_face(segments, vertex_edges, debug):
    points = (p for segment in segments for p in segment)
    p = min(points, key=second)
    start = p
    edges = vertex_edges[p]
    
    edge = edges[0]

    result = []

    max_face_size = 50
    for i in range(max_face_size):
        remove_exactly(edges, edge)
        remove_exactly(segments, edge)
        result.append(p)

        if debug and False:
            draw_line(edge[0], edge[1], color='gray')
            draw_circle(p, 5)
            write(str(i), edge.middle())

        p0 = p
        p = edge.other(p)
        edges = vertex_edges[p]
        index = find_exactly(edges, edge)
        assert(edges[index] is edge)
        del edges[index]
        
        if p == start:
            #print("Reached goal!")
            break

        # find a previous edge that does not go back
        for j in range(len(edges)):
            prev_edge = edges[index - j - 1]
            if prev_edge.other(p) != p0:
                break
        else:
            raise Exception("Could not find previous edge of", edge, "in", edges)
        # remove incomming edge
        edge = prev_edge
    else:
        raise Exception("max_face_size reached")

    return result

def group_segments_by_vertex(segments):
    vertex_edges = defaultdict(list)
    for segment in segments:
        a, b = segment
        vertex_edges[a].append(segment)
        vertex_edges[b].append(segment)

    # sort circular around vertex p
    for p, edges in vertex_edges.items():
        def key(edge):
            return (edge.other(p) - p).angle()
        edges.sort(key=key)

        # degree of each vertex must be even
        assert(len(edges) & 1 == 0)

    return vertex_edges

def unleash_face_eater(segments, vertex_edges):
    #for i in range(4):
    while segments:
        try:
            points = eat_face(segments, vertex_edges, 0)
        except Exception as e:
            traceback.print_exc()

        draw_polygon(points)
        
        if 0:
            a = points[-1]
            for i, b in enumerate(points):
                write(str(i), lerp(a, b, 0.3) + (b-a).scaled(20).left())
                draw_line(a, b)
                a = b

def triangulate(mouse, points):
    draw_polygon(points, '#222222')

    segments = segments_from_points(points)
    
    segments = handle_self_intersection(segments)

    segments = sweep(segments)

    segments = remove_zero_length_segments(segments)

    vertex_edges = group_segments_by_vertex(segments)

    unleash_face_eater(segments, vertex_edges)

import Tkinter as tk
import random
import colorsys
import traceback

width = 800
height = 800

with open("star.txt", "rb") as f:
    s = f.read()
    xy = list(map(float, s.split()))
    xs = xy[0::2]
    ys = xy[1::2]
    points = [Point(Fraction(x), Fraction(y)) for x, y in zip(xs, ys)]

xy = [
    100, 100,
    200, 200,
    300, 100,
    400, 200,
    500, 100,
    600, 200,
    700, 100,
    700, 500,
    100, 500,
]
xs = xy[0::2]
ys = xy[1::2]
#points = [Point(x, y) for x, y in zip(xs, ys)]
"""
random.seed(1)
points = []
while len(points) < 5:
    x = random.randint(0, width)
    y = random.randint(0, height)
    p = Point(x, y)

    if not points or min(p.dist(q) for q in points) > 10:
        points.append(p)
"""
master = tk.Tk()
canvas = tk.Canvas(master, width=width, height=height)
canvas.pack()

def draw_grid(dx=100, dy=100, color='gray'):
    for x in range(0, width, dx):
        canvas.create_line(x, 0, x, height, dash=(4, 4), fill=color)
    for y in range(0, height, dy):
        canvas.create_line(0, y, width, y, dash=(4, 4), fill=color)

def write(text, p, color='white', font=("Purisa", 20)):
    canvas.create_text(float(p[0]), height - 1 - float(p[1]), text=text, fill=color, font=font)

def draw_circle(center, radius, color='white', n=100):
    a = center + Point(radius, 0)
    draw_arc(Arc(center, radius, a, a), color)

def draw_line(a, b, color='white'):
    canvas.create_line(float(a[0]), height - 1 - float(a[1]), float(b[0]), height - 1 - float(b[1]), fill=color)

def draw_arc(arc, color='white', n = 100):
    points = arc.points(n)
    for a, b in zip(points, points[1:]):
        draw_line(a, b, color)

def draw_polygon(points, color='white'):
    xy = flatten_once((float(p.x), height - 1 - float(p.y)) for p in points)
    canvas.create_polygon(xy, fill=color)

def redraw(mouse=Point(width/2, height/2)):
    canvas.delete('all')
    canvas.create_rectangle(0, 0, width, height, fill='black')
    draw_grid()

    try:
        triangulate(mouse, points)
    except Exception as e:
        traceback.print_exc()

def on_drag_left(event):
    mouse = Point(event.x, height - 1 - event.y)

    closest = min(points, key=lambda p: mouse.dist(p))
    closest.is_now(mouse)
    
    redraw(mouse)

redraw()

def on_closing():

    with open("points.txt", "wb") as f:
        s = "\n".join(str(p.x) + " " + str(p.y) for p in points)
        f.write(s)
    
    master.destroy()

master.protocol("WM_DELETE_WINDOW", on_closing)

canvas.bind("<B1-Motion>", on_drag_left)

tk.mainloop()
