import time
from matplotlib import pyplot as plt
from fractions import Fraction
import traceback

EVENT_START        = 1
EVENT_INTERSECTION = 2
EVENT_STOP         = 3

class EventPoint(object):
    def __init__(self, x, y, segment, event_type = 0):
        self.x = x
        self.y = y
        self.segment = segment
        self.event_type = event_type

    def __str__(self):
        return "(" + str(self.x) + ", " + str(self.y) + ", " + str(self.event_type) + ")"

    def __lt__(self, other):
        if self.x == other.x:
            if self.y == other.y:
                return self.event_type < other.event_type
            return self.y < other.y
        return self.x < other.x
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y and self.event_type == other.event_type

current_x = Fraction(0)

class Segment(object):
    def __init__(self, ax, ay, bx, by):
        a = EventPoint(ax, ay, self)
        b = EventPoint(bx, by, self)
        if a > b:
            a, b = b, a
        a.event_type = EVENT_START
        b.event_type = EVENT_STOP
        self.a = a
        self.b = b
        self.last = a
    """
    def is_vertical(self):
        return self.a.x == self.b.x
    """
    def __str__(self):
        return "(" + str(self.a) + ", " + str(self.b) + ")"

    def slope(self):
        a, b = self.a, self.b
        slope = (b.y - a.y)/(b.x - a.x)
        return slope

    def current_y(self):
        a, b = self.a, self.b
        slope = (b.y - a.y)/(b.x - a.x)
        y_intercept = a.y - slope*a.x
        global current_x
        return slope*current_x + y_intercept
    
    def __lt__(self, other):
        self_y = self.current_y()
        other_y = other.current_y()
        if self_y == other_y:
            # TODO division by zero
            return self.slope() < other.slope()
        return self_y < other_y

sorted_lines = []
event_points = []
"""
points = [
    [100, 100],
    [150, 150],
    [200, 50],
    [300, 250],
]
"""
import random
random.seed(1)
points = [(Fraction(random.randint(0, 1000)), Fraction(random.randint(0, 1000))) for _ in range(10)]

a = points[-1]
for b in points:
    segment = Segment(a[0], a[1], b[0], b[1])
    event_points.append(segment.a)
    event_points.append(segment.b)
    a = b

event_points.sort()

output_segments = []

n_already_existed = 0
n_intersections = 0

def intersect(seg_ab, seg_cd):
    a = seg_ab.a
    b = seg_ab.b
    c = seg_cd.a
    d = seg_cd.b

    # TODO check if consecutive lines instead?
    if a == c or a == d or b == c or b == d:
        return

    bax = b.x - a.x
    bay = b.y - a.y
    dcx = d.x - c.x
    dcy = d.y - c.y

    det = bax*dcy - bay*dcx

    # TODO
    assert(det != 0)

    x = c.x - a.x
    y = c.y - a.y

    t = (x*bay - y*bax)/det
    s = (x*dcy - y*dcx)/det

    if t >= 0 and t <= 1 and s >= 0 and s <= 1:
        qx = a.x + s*bax
        qy = a.y + s*bay

        # TODO ?
        if current_x >= qx:
            return

        point = EventPoint(qx, qy, [seg_ab, seg_cd], EVENT_INTERSECTION)
        if point not in event_points:
            event_points.append(point)
            event_points.sort()

            plt.plot([qx], [qy], 'ro')
            global n_intersections
            n_intersections += 1
        else:
            global n_already_existed
            n_already_existed += 1

        # TODO only output a single segment if t = 1 or s = 1

def check(i, j):
    if i < 0 or i >= len(sorted_lines): return
    if j < 0 or j >= len(sorted_lines): return
    intersect(sorted_lines[i], sorted_lines[j])

try:
    while len(event_points) > 0:
        event_point = event_points[0]
        event_points = event_points[1:]
        print("remove " + str(event_point))

        current_x = event_point.x
        segment = event_point.segment
        if event_point.event_type == EVENT_START:
            sorted_lines.append(segment)
            sorted_lines.sort()
            i = sorted_lines.index(segment)
            check(i-1, i)
            check(i+1, i)
        elif event_point.event_type == EVENT_STOP:
            i = sorted_lines.index(segment)
            sorted_lines.remove(segment)
            output_segments.append((segment.last, event_point))
            check(i, i-1) 
        elif event_point.event_type == EVENT_INTERSECTION:
            seg_a, seg_b = segment
            output_segments.append((seg_a.last, event_point))
            output_segments.append((seg_b.last, event_point))
            seg_a.last = event_point
            seg_b.last = event_point

            # TODO this more efficient and corret
            sorted_lines.sort()
            if len(sorted_lines) > 1:
                a = sorted_lines[0]
                for b in sorted_lines[1:]:
                    intersect(a, b)
                    a = b
        else:
            raise Exception("Unknown event type")

        """
        for i in range(len(sorted_lines)):
            for j in range(i + 1, len(sorted_lines)):
                intersect(sorted_lines[i], sorted_lines[j])
        """

except Exception as e:
    traceback.print_exc()
    for line in sorted_lines:
        print(line)

def draw_segment(seg):
    a = seg[0]
    b = seg[1]
    line, = plt.plot([a.x, b.x], [a.y, b.y])
    line.set_dashes([1, 3])

for segment in output_segments:
    draw_segment(segment)

print(n_already_existed, " intersections of ", n_intersections, "already existed")

plt.show()
