function Segment(a, b){
	this.a = a
	this.b = b
}

Segment.prototype.stroke = function(){
	strokeLine(this.a, this.b)
}

Segment.prototype.lerp = function(u){
	return lerp(this.a, this.b, u)
}

Segment.prototype.middle = function(u){
	return smul(0.5, add(this.a, this.b))
}

Segment.prototype.draw = function(fill){
	var a = this.a
	var b = this.b
	context.beginPath()
	context.moveTo(a[0], a[1])
	context.lineTo(b[0], b[1])
	if (fill) context.fill()
	else context.stroke()
}
/*
function closestPointToSegment(p, a, b){
	var ba = sub(b, a)
	var pa = sub(p, a)
	var t = dot(pa, ba)/dot(ba, ba)
	var u = clamp(t, 0, 1)
	var q = add(a, smul(u, ba))
	return q
}
*/
