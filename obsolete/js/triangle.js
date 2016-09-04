function Triangle(a, b, c){
	this.a = a
	this.b = b
	this.c = c
}

Triangle.prototype.contains = function(){
	var b0 = isLeftOf(p, this.a, this.b)
	var b1 = isLeftOf(p, this.b, this.c)
	var b2 = isLeftOf(p, this.c, this.a)
	return (b0 == b1) && (b1 == b2)
}

Triangle.prototype.intersectionsSegment = function(seg){
	var s0 = intersectionsSegmentSegment(new Segment(this.a, this.b), seg)
	var s1 = intersectionsSegmentSegment(new Segment(this.b, this.c), seg)
	var s2 = intersectionsSegmentSegment(new Segment(this.c, this.a), seg)
	return filter(identity, [].concat(s0, s1, s2))
}

Triangle.prototype.intersectionsTriangle = function(tri){
	var s0 = this.intersectionsSegment(new Segment(tri.a, tri.b))
	var s1 = this.intersectionsSegment(new Segment(tri.b, tri.c))
	var s2 = this.intersectionsSegment(new Segment(tri.c, tri.a))
	return filter(identity, [].concat(s0, s1, s2))
}
