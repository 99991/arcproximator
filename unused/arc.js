function Arc(center, radius, a, b, counterclockwise){
	if (typeof a == "number"){
		a = add(center, polar(a, radius))
	}
	if (typeof b == "number"){
		b = add(center, polar(b, radius))
	}
	this.center = center
	this.a = a
	this.b = b
	this.radius = radius
	this.counterclockwise = counterclockwise
}

Arc.prototype.draw = function(fill){
	var a = this.a
	var b = this.b
	var center = this.center
	var aAngle = Math.atan2(a[1] - center[1], a[0] - center[0])
	var bAngle = Math.atan2(b[1] - center[1], b[0] - center[0])
	context.beginPath()
	// counterclockwise is flipped because default canvas is upside down
	context.arc(center[0], center[1], this.radius, aAngle, bAngle, !this.counterclockwise)
	//context.closePath()
	if (fill) context.fill()
	else context.stroke()
}

// tells if the point p is on the arc, assuming it is on the circle
Arc.prototype.sees = function(p){
	if (this.counterclockwise){
		return isRightOf(p, this.a, this.b)
	}else{
		return isLeftOf(p, this.a, this.b)
	}
}

Arc.prototype.isOver180 = function(p){
	if (this.counterclockwise){
		return isLeftOf(this.b, this.a, this.center)
	}else{
		return isRightOf(this.b, this.a, this.center)
	}
}

Arc.prototype.isOver90AndLessThan270 = function(p){
	return isInFrontOf(this.b, this.a, this.center)
}
/*
Arc.prototype.getPoints = function(n, t0, t1){
	if (t0 === undefined) t0 = 0
	if (t1 === undefined) t1 = 1
	
	var center = this.center
	var radius = this.radius
	
	var startAngle = atan(sub(this.start, center))
	var endAngle = atan(sub(this.end, center))
	
	if (this.counterclockwise){
		if (endAngle < startAngle) endAngle += Math.PI*2
	}else{
		if (startAngle < endAngle) startAngle += Math.PI*2
	}
	
	if (n == 1){
		var angle = lerp(startAngle, endAngle, t0)
		return [add(center, polar(angle, radius))]
	}
	
	var points = new Array(n)
	for (var i = 0; i < n; i++){
		var u = t0 + (t1 - t0)*i/(n - 1)
		var angle = lerp(startAngle, endAngle, u)
		points[i] = add(center, polar(angle, radius))
	}
	return points
}
*/
Arc.prototype.split = function(p){
	return [
		new Arc(this.center, this.radius, this.a, p, this.counterclockwise),
		new Arc(this.center, this.radius, p, this.b, this.counterclockwise),
	]
}

Arc.prototype.middle = function(){
	var center = this.center
	var v0 = sub(this.a, center)
	var v1 = sub(this.b, center)
	var v = scale(add(v0, v1), this.radius)
	if (isLeftOf(this.b, this.a, this.center)){
		if (this.counterclockwise){
			return sub(center, v)
		}else{
			return add(center, v)
		}
	}
	if (isRightOf(this.b, this.a, this.center)){
		if (this.counterclockwise){
			return add(center, v)
		}else{
			return sub(center, v)
		}
	}
	console.log("TODO: exactly 180 degree arc")
}

Arc.prototype.getHull = function(){
	var a = this.a
	var b = this.b
	var center = this.center
	
	var aTangent = left(sub(a, center))
	var bTangent = right(sub(b, center))
	
	var p = intersectionRayRay(a, aTangent, b, bTangent)
	
	return new Triangle(a, b, p)
}
