
function intersectionRayCircle(start, direction, center, radius){
	var v = sub(center, start)
	var temp = dot(v, direction)
	var d2 = temp*temp - dot(v, v) + radius*radius
	if (d2 < 0) return
	var d = Math.sqrt(d2)
	return [temp - d, temp + d]
}

function intersectionCircleCircle(a, aRadius, b, bRadius){
	var ba = sub(b, a)
	var d = len(ba)
	// circles far away
	if (d > aRadius + bRadius) return
	// circle inside
	if (d < Math.abs(aRadius - bRadius)) return
	// same circles
	if (d == 0 && aRadius == bRadius) return
	
	var da = (aRadius*aRadius - bRadius*bRadius + d*d)*0.5/d
	var h = Math.sqrt(aRadius*aRadius - da*da)
	var p = add(a, smul(da/d, ba))
	var v = left(smul(h/d, ba))
	return [add(p, v), sub(p, v)]
}

function intersectionsArcArc(arc0, arc1){
	var intersections = intersectionCircleCircle(arc0.center, arc0.radius, arc1.center, arc1.radius)
	var actualIntersections = []
	if (intersections){
		for (var i = 0; i < intersections.length; i++){
			var p = intersections[i]
			if (arc0.sees(p) && arc1.sees(p)){
				actualIntersections.push(p)
			}
		}
	}
	return actualIntersections
}

function intersectionsArcSegment(arc, segment){
	var a = segment.a
	var b = segment.b
	var ba = sub(b, a)
	var d = len(ba)
	var direction = smul(1.0/d, ba)
	var actualIntersections = []
	var intersections = intersectionRayCircle(a, direction, arc.center, arc.radius)
	if (intersections){
		for (var i = 0; i < intersections.length; i++){
			var t = intersections[i]
			if (t >= 0 && t <= d){
				var p = add(a, smul(t/d, ba))
				if (arc.sees(p)){
					actualIntersections.push(p)
				}
			}
		}
	}
	return actualIntersections
	
}

/*

function intersectionSegmentSegment(a, b, c, d){
	var bax = b[0] - a[0]
	var bay = b[1] - a[1]
	var dcx = d[0] - c[0]
	var dcy = d[1] - c[1]
	var det = bax*dcy - bay*dcx
	
	if (det === 0){
		console.log("TODO: parallel lines")
	}

	var x = c[0] - a[0]
	var y = c[1] - a[1]
	
    var t = (x*bay - y*bax)/det
    var s = (x*dcy - y*dcx)/det

	if (t >= 0 && t <= 1 && s >= 0 && s <= 1){
        var qx = a.x + s*bax
        var qy = a.y + s*bay
        return [qx, qy]
	}
}
*/

// TODO very similar to intersectionSegmentSegment
function intersectionsSegmentSegment(ab, cd){
	var a = ab.a
	var b = ab.b
	var c = cd.a
	var d = cd.b
	
	var bax = b[0] - a[0]
	var bay = b[1] - a[1]
	var dcx = d[0] - c[0]
	var dcy = d[1] - c[1]
	
	var det = bax*dcy - bay*dcx
	
	if (det === 0){
		//console.log("TODO: parallel lines")
	}

	var x = c[0] - a[0]
	var y = c[1] - a[1]
	
    var t = (x*bay - y*bax)/det
    var s = (x*dcy - y*dcx)/det

	if (t >= 0 && t <= 1 && s >= 0 && s <= 1){
        var qx = a[0] + s*bax
        var qy = a[1] + s*bay
        return [[qx, qy]]
	}
}

function intersectionRayRay(a, ba, c, dc){
	var det = ba[0]*dc[1] - ba[1]*dc[0]
	
	if (det === 0){
		console.log("TODO: parallel lines")
	}

	var x = c[0] - a[0]
	var y = c[1] - a[1]
	
    var s = (x*dc[1] - y*dc[0])/det
    
	var qx = a[0] + s*ba[0]
	var qy = a[1] + s*ba[1]
	
	return [qx, qy]
}
