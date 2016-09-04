
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

function intersectionsArcArc(arc0, arc1, actualIntersections){
	var intersections = intersectionCircleCircle(arc0.center, arc0.radius, arc1.center, arc1.radius)
	if (intersections){
		for (var i = 0; i < intersections.length; i++){
			var p = intersections[i]
			if (arc0.sees(p) && arc1.sees(p)){
				actualIntersections.push(p)
			}
		}
	}
}

function intersectionsArcSegment(arc, segment, actualIntersections){
	var a = segment.a
	var b = segment.b
	var ba = sub(b, a)
	var d = len(ba)
	var direction = smul(1.0/d, ba)
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
function intersectionsSegmentSegment(ab, cd, intersections){
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
        intersections.push([qx, qy])
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



function countArcIntersections(x, y, arc){
	var clockwise = !arc.counterclockwise
	var a = arc.a
	var b = arc.b
	
	// cw case is same as ccw case with 'a' and 'b' swapped
	if (clockwise){
		var temp = a
		a = b
		b = temp
	}
	
	var center = arc.center
	var radius = arc.radius
	var ax = a[0]
	var ay = a[1]
	var bx = b[0]
	var by = b[1]
	var cx = center[0]
	var cy = center[1]
	
	// bounding box of circle
	var right = cx + radius
	var bottom = cy - radius
	var top = cy + radius
	
	// if ray does not intersect bounding box
	if (y < bottom || y > top || x > right) return 0
	
	var outside = dist2([x, y], center) > radius*radius
	
	// if ray does not intersect circle
	if (outside && x > cx) return 0
	
	if (ay == by){
		// TODO
	}
	
	if (ay < by){
		// 'b' above 'a'
		
		// TODO ax == cx or ay == cy
		
		if (ax <= cx && bx <= cx){
			if ((y < ay || y >= by) && outside) return 2
			return 1
		}
		if (ax > cx && bx > cx){
			if (ay <= y && y < by) return 1
			return 0
		}
		if (ax > cx && bx <= cx){
			if (y >= by) return outside ? 2 : 1
			if (y >= ay) return 1
			return 0
		}
		//if (ax <= cx && bx > cx)
		{
			if (y < ay) return outside ? 2 : 1
			if (y < by) return 1
			return 0
		}
	}else{
		// 'a' above 'b'
		
		// TODO ax == cx or bx == cx
		
		if (ax <= cx && bx <= cx){
			if (by <= y && y < ay && outside) return 1
			return 0
		}
		if (ax > cx && bx > cx){
			if (y >= ay || y < by) return outside ? 2 : 1
			return outside ? 1 : 0
		}
		if (ax > cx && bx <= cx){
			if (y >= ay) return outside ? 2 : 1
			if (y >= by && outside) return 1
			return 0
		}
		//if (ax <= cx && bx > cx)
		{
			if (y < by) return outside ? 2 : 1
			if (y < ay && outside) return 1
			return 0
		}
	}
}


function getArcIntersections(x, y, arc, intersections){
	var p = [x, y]
	var center = arc.center
	var radius = arc.radius
	var a = arc.a
	var b = arc.b
	
	if (arc.counterclockwise){
		var temp = a
		a = b
		b = temp
	}
	
	var cx = center[0]
	var dx = cx - x
	var dx2 = dx*dx
	var d2 = dx2 - dist2(p, center) + radius*radius
	
	if (d2 >= 0){
		var d = Math.sqrt(d2)
		var d0 = dx - d
		var d1 = dx + d
		var p0 = [x + d0, y]
		var p1 = [x + d1, y]
		
		if (isLeftOfOrOn(p0, a, b)){
			intersections.push(p0)
		}
		if (isLeftOfOrOn(p1, a, b)){
			intersections.push(p1)
		}
		
		/*
		if (isLeftOf(p0, a, b)){
			if (dx >= 0){
				if (dx2 >= +d2) intersections.push(p0)
			}else{
				if (-dx2 >= +d2) intersections.push(p0)
			}
		}
		if (isLeftOf(p1, a, b)){
			if (dx >= 0){
				if (dx2 >= -d2) intersections.push(p1)
			}else{
				if (-dx2 >= -d2) intersections.push(p1)
			}
		}
		*/
	}
	
	return intersections
}

// shoot a horizontal ray to the right of [rx, ry]
function getSegmentIntersections(rx, ry, segment, intersections){
	var a = segment.a
	var b = segment.b
	
	var ax = a[0]
	var ay = a[1]
	var bx = b[0]
	var by = b[1]
	
	// return early if ray is not between 'a' and 'b'
	if (ry < Math.min(ay, by) || ry > Math.max(ay, by)) return
	
	// ignore horizontal line segments
	if (ay == by) return
	
	var x = ax + (ry - ay)*(ax - bx)/(ay - by)
	
	//if (x >= rx)
	intersections.push([x, ry])
}

function countArcIntersections2(x, y, arc){
	var intersections = []
	getArcIntersections(x, y, arc, intersections)
	return intersections.length
}

function countSegmentIntersections(x, y, segment){
	var a = segment.a
	var b = segment.b
	
	if (a[1] > b[1]){
		var temp = a
		a = b
		b = temp
	}
	
	var ax = a[0]
	var ay = a[1]
	var bx = b[0]
	var by = b[1]
	
	// ignore horizontal line segments and intersections in 'b'
	if (ay != by && ay <= y && y < by){
		if ((x - ax)*(by - ay) - (y - ay)*(bx - ax) <= 0){
			return 1
		}
	}
	return 0
}


function isInGeneralPolygon(x, y, curves){
	var nIntersections = 0
	for (var i = 0; i < curves.length; i++){
		var curve = curves[i]
		switch (curve.constructor){
			case Segment:
				nIntersections += countSegmentIntersections(x, y, curve);
			break
			case Arc:
				nIntersections += countArcIntersections(x, y, curve);
			break
		}
	}
	
	return nIntersections & 1 == 1
}

function isEdgeCase(curves){
	for (var y = 0; y < ny; y++){
		if (isInGeneralPolygon(-1000, y, curves)){
			return true
		}
	}
	return false
}
