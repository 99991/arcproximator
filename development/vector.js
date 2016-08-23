
function isLeftOf(p, a, b){
	return (p[0] - a[0])*(b[1] - a[1]) - (p[1] - a[1])*(b[0] - a[0]) < 0
}

function isLeftOfOrOn(p, a, b){
	return (p[0] - a[0])*(b[1] - a[1]) - (p[1] - a[1])*(b[0] - a[0]) <= 0
}

function isRightOf(p, a, b){
	return (p[0] - a[0])*(b[1] - a[1]) - (p[1] - a[1])*(b[0] - a[0]) > 0
}

function isInFrontOf(p, a, b){
	return dot(sub(p, b), sub(a, b)) < 0
}

function dot(a, b){
	return a[0]*b[0] + a[1]*b[1]
}

function det(a, b){
	return a[0]*b[1] - a[1]*b[0]
}

function add(a, b){
	return [a[0] + b[0], a[1] + b[1]]
}

function sub(a, b){
	return [a[0] - b[0], a[1] - b[1]]
}

function left(a){
	return [-a[1], a[0]]
}

function right(a){
	return [a[1], -a[0]]
}

function dist2(a, b){
	var dx = a[0] - b[0]
	var dy = a[1] - b[1]
	return dx*dx + dy*dy
}

function round(a){
	return [Math.round(a[0]), Math.round(a[1])]
}

function atan(p){
	return Math.atan2(p[1], p[0])
}

function dist(a, b){
	return Math.sqrt(dist2(a, b))
}

function len2(a){
	return dot(a, a)
}

function len(a){
	return Math.sqrt(len2(a))
}

function invLen(a){
	return 1/len(a)
}

function smul(a, b){
	return [a*b[0], a*b[1]]
}

function scale(a, d){
	return smul(d/len(a), a)
}

function normalize(a){
	return scale(a, 1)
}

function polar(angle, radius){
	return [Math.cos(angle)*radius, Math.sin(angle)*radius]
}

function lerp(a, b, u){
	if (typeof a == "number" && typeof b == "number"){
		return a*(1 - u) + u*b
	}else{
		var s = 1 - u
		var x = a[0]*s + u*b[0]
		var y = a[1]*s + u*b[1]
		return [x, y]
	}
}

function isEqual(a, b){
	return a[0] == b[0] && a[1] == b[1]
}

function diamondAngle(p){
	var x = p[0]
	var y = p[1]
	return y >= 0 ?
		(x >= 0 ? 0 + y/(x + y) : 1 + x/(x - y)) :
		(x <  0 ? 2 + y/(x + y) : 3 + x/(x - y))
}

function quadrant(p){
	if (p[0] >= 0){
		return p[1] >= 0 ? 0 : 3
	}else{
		return p[1] >= 0 ? 1 : 2
	}
}
