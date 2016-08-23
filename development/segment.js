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

Segment.prototype.draw = function(context){
	drawLine(context, this.a, this.b)
}

Segment.prototype.closest = function(p){
	var a = this.a
	var b = this.b
	var pa = sub(p, a)
	var ba = sub(b, a)
	var u = clamp(dot(pa, ba), 0, 1)
	return lerp(a, b, u)
}

Segment.prototype.dist = function(p){
	return dist(p, this.closest(p))
}

Segment.prototype.splitAtPoints = function(points, segments){
	var a = this.a
	var b = this.b
	var ba = sub(b, a)
	
	points = filter(function(p){
		var d = dot(sub(p, a), ba)
		return d > 0 && d < dot(ba, ba)
	}, points)
	
	sortByKey(points, function(p){
		return dot(sub(p, a), ba)
	})
	
	points.push(b)
	for (var i = 0; i < points.length; i++){
		var b = points[i]
		segments.push(new Segment(a, b))
		a = b
	}
}
