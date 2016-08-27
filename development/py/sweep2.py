from matplotlib import pyplot as plt
from fractions import Fraction
import itertools

class Point(tuple):
    def __new__(typ, x, y):
        return tuple.__new__(typ, (x, y))

    def __add__(self, other):
        return Point(self[0] + other[0], self[1] + other[1])

    def __sub__(self, other):
        return Point(self[0] - other[0], self[1] - other[1])

    def dot(self, other):
        return self[0]*other[0] + self[1]*other[1]

    def left(self):
        return Point(-self[1], self[0])

    def dist2(self, other):
        d = self - other
        return d.dot(d)

def calculate_slope(a, b):
    dx = b[0] - a[0]
    dy = b[1] - a[1]

    # must not be -inf so vertical segments
    # that go up or down are grouped together
    if dx == 0: return float("inf")
    
    return Fraction(dy, dx)

def calculate_y(segment, x):
    a, b, slope = segment

    if slope == float("inf"): return a[1]

    y_intercept = a[1] - slope*a[0]

    return slope*x + y_intercept

def intersect(seg_ab, seg_cd):
    a, b = seg_ab
    c, d = seg_cd
    
    bax = b[0] - a[0]
    bay = b[1] - a[1]
    dcx = d[0] - c[0]
    dcy = d[1] - c[1]

    det = bax*dcy - bay*dcx

    # parallel overlapping segments already handled
    if det == 0: return
    
    cax = c[0] - a[0]
    cay = c[1] - a[1]

    t = (cax*bay - cay*bax)/det
    s = (cax*dcy - cay*dcx)/det

    if t >= 0 and t <= 1 and s >= 0 and s <= 1:
        qx = a[0] + s*bax
        qy = a[1] + s*bay

        return [qx, qy]

points = [
    100, 100,
    300, 100,
    200, 100,
    200, 100,
    250, 100,
    300, 300,
    300, 400,
    150, 400,
    150, 300,
    300, 200,
    100, 200,
    100, 400,
    100, 300,
]

points = [Point(x, y) for x, y in zip(points[0::2], points[1::2])]
segments = []
a = points[-1]
for b in points:
    segments.append((a, b, calculate_slope(a, b)))
    a = b

segments = list(filter(lambda seg: seg[0].dist2(seg[1]) > 0, segments))

def intersect_parallel_segments(segments, slope):
    if len(segments) <= 1: return segments
    
    a_points = [(seg[0], seg) for seg in segments]
    b_points = [(seg[1], seg) for seg in segments]
    points = a_points + b_points

    a, b, _ = segments[0]
    direction = b - a
    points = [(p.dot(direction.left()), p.dot(direction), p, seg) for p, seg in points]
    points.sort()
    points = [(p, seg) for _, _, p, seg in points]

    # The following could be done in O(n) instead of O(n log n),
    # but the sorting above was O(n log n) already, so not much to win here
    current_segments = set()
    new_segments = []
    segment_start = None
    for point, segment in points:
        if segment in current_segments:
            # end of segment
            current_segments.remove(segment)
        else:
            # start of segment
            current_segments.add(segment)

        # start a new segment
        if segment_start and segment_start != point:
            new_segments.append((segment_start, point, slope))

        # start of next segment is end of previous segment
        segment_start = point if current_segments else None

    return new_segments

segments.sort(key = lambda seg: seg[2])
grouped_segments = [intersect_parallel_segments(list(group), slope) for slope, group in itertools.groupby(segments, lambda seg: seg[2])]
segments = itertools.chain(*grouped_segments)

for a, b, slope in segments:
    plt.plot([a[0], b[0]], [a[1], b[1]], 'x-')

active_segments = []
current_x = 0
a_points = [seg[0] for seg in segments]
b_points = [seg[1] for seg in segments]
event_points = a_points + b_points

def compare_segments(a, b):
    # TODO
    pass

axes = plt.gca()
axes.set_xlim([0, 500])
axes.set_ylim([0, 500])
plt.show()
