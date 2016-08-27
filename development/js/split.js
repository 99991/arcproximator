
function splitArcAtPoints(arc, points){
	var center = arc.center
	var ccw = arc.counterclockwise
	var aAngle = diamondAngle(sub(arc.a, center))
	
	sortByKey(points, function(p){
		var angle = diamondAngle(sub(p, center))
		if (angle < aAngle) angle += 4
		if (!ccw) angle = -angle
		return angle
	})
	
	var arcs = []
	var a = arc.a
	for (var i = 0; i < points.length; i++){
		var b = points[i]
		arcs.push(new Arc(center, arc.radius, a, b, ccw))
		a = b
	}
	arcs.push(new Arc(center, arc.radius, a, arc.b, ccw))
	return arcs
}

function splitSegmentAtPoints(segment, points){
	var a = segment.a
	var b = segment.b
	
	sortByKey(points, function(p){
		return dot(sub(p, a), sub(b, a))
	})
	
	var segments = []
	for (var i = 0; i < points.length; i++){
		var b = points[i]
		segments.push(new Segment(a, b))
		a = b
	}
	segments.push(new Segment(a, segment.b))
	return segments
}

function splitLongArcs(curves){
	var newCurves = []
	var n = curves.length
	for (var i = 0; i < n; i++){
		var curve = curves[i]
		if (curve.constructor == Arc){
			var arc = curve
			var center = arc.center
			var radius = arc.radius
			var counterclockwise = arc.counterclockwise
			
			if (arc.isOver180()){
				// split into 4 parts
				var arcs = arc.split(arc.middle())
				append(newCurves, arcs[0].split(arcs[0].middle()))
				append(newCurves, arcs[1].split(arcs[1].middle()))
			}else if (arc.isOver90AndLessThan270()){
				// split into 2 parts
				append(newCurves, arc.split(arc.middle()))
			}else{
				// don't split
				newCurves.push(arc)
			}
		}else{
			newCurves.push(curve)
		}
	}
	return newCurves
}

function splitIntersectingArcs(curves){
	var maxIterations = 5
	for (var iteration = 0; iteration < maxIterations; iteration++){
		var n = curves.length
		var marked = new Array(n)
		for (var i = 0; i < n; i++) marked[i] = false
		
		// mark curves for splitting
		for (var i = 0; i < n; i++){
			var curve0 = curves[i]
			if (curve0.constructor == Arc){
				var a = curve0.a
				var b = curve0.b
				var hull = curve0.getHull()
				
				context.strokeStyle = "gray"
				drawPolygon([hull.a, hull.b, hull.c])
				
				// TODO mark curves which have a hull that contains points
				jLoop: 
				for (var j = 0; j < n; j++){
					if (i == j) continue
					var curve1 = curves[j]
					var intersections;
					switch (curve1.constructor){
						case Arc:
							intersections = hull.intersectionsTriangle(curve1.getHull())
						break
						case Segment:
							intersections = hull.intersectionsSegment(curve1)
						break
					}
					for (var k = 0; k < intersections.length; k++){
						var p = intersections[k]
						if (almostEqualPoints(p, a) || almostEqualPoints(p, b)) continue
						marked[i] = true
						if (curve1.constructor == Arc){
							marked[j] = true
						}
						drawCircle(p, 10)
						break jLoop
					}
				}
			}
		}
		
		var nMarks = 0
		for (var i = 0; i < n; i++) nMarks += marked[i]
		if (nMarks && iteration == maxIterations - 1){
			console.log("WARNING: max iterations reached")
		}
		
		var newCurves = []
		for (var i = 0; i < n; i++){
			var curve = curves[i]
			if (marked[i]){
				append(newCurves, curve.split(curve.middle()))
			}else{
				newCurves.push(curve)
			}
		}
		curves = newCurves
	}
	
	return curves
}

function splitAtIntersections(curves){
	var n = curves.length
	
	var splitPoints = new Array(n)
	for (var i = 0; i < n; i++) splitPoints[i] = []
	
	var count = 0
	for (var i = 0; i < n; i++){
		var curve0 = curves[i]
		for (var j = i + 1; j < n; j++){
			var curve1 = curves[j]
			
			var intersections;
			
			if (curve0.constructor == Arc && curve1.constructor == Arc){
				intersections = intersectionsArcArc(curve0, curve1)
			}
			if (curve0.constructor == Arc && curve1.constructor == Segment){
				intersections = intersectionsArcSegment(curve0, curve1)
			}
			if (curve0.constructor == Segment && curve1.constructor == Arc){
				intersections = intersectionsArcSegment(curve1, curve0)
			}
			if (curve0.constructor == Segment && curve1.constructor == Segment){
				intersections = intersectionsSegmentSegment(curve1, curve0)
			}
			
			append(splitPoints[i], intersections)
			append(splitPoints[j], intersections)
		}
	}
	
	context.strokeStyle = "black"
			
	var newCurves = []
	for (var i = 0; i < n; i++){
		var curve = curves[i]
		var points = splitPoints[i]
		switch (curve.constructor){
			case Arc:
				append(newCurves, splitArcAtPoints(curve, points))
			break
			case Segment:
				append(newCurves, splitSegmentAtPoints(curve, points))
			break
		}
		
		for (var j = 0; j < points.length; j++){
			var p = points[j]
			drawCircle(p, 5)
		}
	}
	return newCurves
}
