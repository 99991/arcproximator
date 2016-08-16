
Arc.prototype.fill = function(){
	var start = this.start
	var end = this.end
	var center = this.center
	var startAngle = Math.atan2(start[1] - center[1], start[0] - center[0])
	var endAngle = Math.atan2(end[1] - center[1], end[0] - center[0])
	context.beginPath()
	// counterclockwise is flipped because default canvas is upside down
	context.arc(center[0], center[1], this.radius, startAngle, endAngle, !this.counterclockwise)
	context.fill()
}

Arc.prototype.sweeps = function(p){
	var center = this.center
	var start = this.start
	var end = this.end
	if (this.counterclockwise){
		if (isLeftOf(end, start, center)){
			// over 180 degrees
			return isLeftOf(p, center, start) || isRightOf(p, center, end)
		}else{
			// less than 180 degrees
			return isLeftOf(p, center, start) && isRightOf(p, center, end)
		}
	}else{
		// clockwise
		if (isLeftOf(end, start, center)){
			// less than 180 degrees
			return isRightOf(p, center, start) && isLeftOf(p, center, end)
		}else{
			// over 180 degrees
			return isRightOf(p, center, start) || isLeftOf(p, center, end)
		}
	}
}

Arc.prototype.contains = function(p){
	if (this.counterclockwise){
		if (isLeftOf(p, this.start, this.end)) return false
	}else{
		if (isRightOf(p, this.start, this.end)) return false
	}
	
	var radius = this.radius
	return dist2(p, this.center) <= radius*radius
}

function splitArcs(arcs, p){
	var newArcs = []
	for (var i = 0; i < arcs.length; i++){
		var arc = arcs[i]
		if (arc.sees(p) && !almostEqualPoints(arc.start, p) && !almostEqualPoints(arc.end, p)){
			var arc0 = new Arc(arc.center, arc.radius, arc.start, p, arc.counterclockwise)
			var arc1 = new Arc(arc.center, arc.radius, p, arc.end, arc.counterclockwise)
			newArcs.push(arc0)
			newArcs.push(arc1)
		}else{
			newArcs.push(arc)
		}
	}
	return newArcs
}

function splitSegments(segments, p){
	var newSegments = []
	for (var i = 0; i < segments.length; i++){
		var segment = segments[i]
		var a = segment[0]
		var b = segment[1]
		var ba = sub(b, a)
		var pa = sub(p, a)
		var u = dot(pa, ba)
		if (u >= 0 && u <= dot(ba, ba) && !almostEqualPoints(a, p) && !almostEqualPoints(b, p)){
			newSegments.push([a, p])
			newSegments.push([p, b])
		}else{
			newSegments.push(segment)
		}
	}
	return newSegments
}

if (0){
	// circular sorting
	var n = 100
	var center = [256, 256]
	var radius = 200
	var points = new Array(n)
	var start = add(center, scale(sub(mouse, center), radius))
	fillCircle(start, 5)
	for (var i = 0; i < n; i++){
		var offset = polar(Math.PI*2*i/n, radius)
		points[i] = add(center, offset)
	}
	shuffle(points)
	sortPointsAroundPoint(points, center, start, true)
	for (var i = 0; i < n; i++){
		var p = points[i]
		write(i+"", p)
	}
}

if (0){
	// diamond angle
	var n = 16
	var center = [256, 256]
	var radius = 100
	for (var i = 0; i < n; i++){
		var offset = polar(Math.PI*2*i/n, radius)
		var p = add(center, offset)
		var angle = diamondAngle(offset)
		write(angle.toFixed(3), p)
	}
}

if (0){
	// arc vs line
	context.lineWidth = 3
	var arc = makeArc(mouse, 100, 0.5, 1.5)
	
	var start = [100, 300]
	var end = [450, 200]
	var d = dist(start, end)
	var direction = normalize(sub(end, start))
	var intersections = intersectionRayCircle(start, direction, arc.center, arc.radius);
	
	if (intersections){
		var d0 = intersections[0]
		var d1 = intersections[1]
		var arcs = [arc]
		var segments = [[start, end]]
		
		if (d0 >= 0 && d0 <= d){
			var p = add(start, smul(d0, direction))
			if (arc.sees(p)){
				fillCircle(p, 5)
				arcs = splitArcs(arcs, p)
				segments = splitSegments(segments, p)
			}
		}
		if (d1 >= 0 && d1 <= d){
			var p = add(start, smul(d1, direction))
			if (arc.sees(p)){
				fillCircle(p, 5)
				arcs = splitArcs(arcs, p)
				segments = splitSegments(segments, p)
			}
		}
		
		var j = 0
		for (var i = 0; i < arcs.length; i++){
			var arc = arcs[i]
			context.strokeStyle = colors[j++]
			arc.stroke()
		}
		for (var i = 0; i < segments.length; i++){
			var segment = segments[i]
			context.strokeStyle = colors[j++]
			strokeLine(segment[0], segment[1])
		}
	}else{		
		context.strokeStyle = "black"
		arc.stroke()
		strokeLine(start, end)
	}
}

if (0){
	// arc vs arc
	context.lineWidth = 3
	var arc0 = makeArc(mouse, 100, 0.5, 1.5)
	var arc1 = makeArc([256, 256], 150, -0.3, -0.9, true)
	
	var intersections = intersectionCircleCircle(arc0.center, arc0.radius, arc1.center, arc1.radius)
	
	if (intersections){
		var a = intersections[0]
		var b = intersections[1]
		var arcs0 = [arc0]
		var arcs1 = [arc1]
		if (arc0.sees(a) && arc1.sees(a)){
			context.fillStyle = "red"
			fillCircle(a, 5)
			arcs0 = splitArcs(arcs0, a)
			arcs1 = splitArcs(arcs1, a)
		}
		if (arc0.sees(b) && arc1.sees(b)){
			context.fillStyle = "green"
			fillCircle(b, 5)
			arcs0 = splitArcs(arcs0, b)
			arcs1 = splitArcs(arcs1, b)
		}

		var j = 0
		for (var i = 0; i < arcs0.length; i++){
			var arc = arcs0[i]
			context.strokeStyle = colors[j++]
			arc.stroke()
		}
		for (var i = 0; i < arcs1.length; i++){
			var arc = arcs1[i]
			context.strokeStyle = colors[j++]
			arc.stroke()
		}
	}else{		
		context.strokeStyle = "black"
		arc0.stroke()
		arc1.stroke()
	}
}

if (0){		
	// circle vs ray
	var radius = 100
	var center = [200, 300]
	var start = [300, 400]
	var direction = normalize(sub(mouse, start))
	var d = intersectionRayCircle(start, direction, center, radius)
	if (d){
		var a = add(start, smul(d[0], direction))
		var b = add(start, smul(d[1], direction))
		fillCircle(a, 5)
		fillCircle(b, 5)
		strokeLine(start, add(start, smul(1000, direction)))
	}
	strokeCircle(center, radius)
}

if (0){
	// circle vs circle
	var a = [200, 200]
	var b = mouse
	var radius = 150
	strokeCircle(a, radius)
	strokeCircle(b, radius)
	var intersections = intersectionCircleCircle(a, radius, b, radius);
	if (intersections){
		fillCircle(intersections[0], 5)
		fillCircle(intersections[1], 5)
	}
}

if (0){
	var radius = 100
	var center = [200, 300]
	var start = add(center, polar(0.4, radius))
	var mc = sub(mouse, center)
	var end = add(center, scale(mc, radius))
	var arc = new Arc(center, radius, start, end, true)
	
	strokeLine(start, end)
	console.log(start, end)
	context.fillStyle = "green"
	var r = 2
	for (var x = 0; x < canvas.width; x += 2*r){
		for (var y = 0; y < canvas.height; y+= 2*r){
			p = [x, y]
			if (arc.contains(p)){
				fillCircle(p, r)
			}
		}
	}
	
	arc.stroke()
}

	
if (0){
	// test isInFrontOf
	var a = mouse
	var b = [256, 256]
	var r = 5
	for (var x = 0; x < canvas.width; x += 2*r){
		for (var y = 0; y < canvas.height; y+= 2*r){
			var p = [x, y]
			if (isInFrontOf(p, a, b)){
				fillCircle(p, r)
			}
		}
	}
}

if (0){
	// arc vs segment
	var arc = makeArc(mouse, 100, 0.2, 2.5, true)
	
	var a = [100, 300]
	var b = [450, 200]
	
	var intersections = intersectionsArcSegment(arc, a, b)
	context.strokeStyle = "black"
	for (var i = 0; i < intersections.length; i++){
		fillCircle(intersections[i], 5)
	}
	arc.stroke()
	strokeLine(a, b)
	
	subdivide(arc, a, b, 5)
}

if (0){
	// intersect multiple arcs
	var arc0 = makeArc(mouse, 100, 0.5, 1.5)
	var arc1 = makeArc([256, 256], 150, -0.3, -0.9, true)
	var arc2 = makeArc([300, 400], 125, -0.3, -1.9)
	var arcs = [arc0, arc1, arc2]
	arcs = intersectArcs(arcs)
	for (var i = 0; i < arcs.length; i++){
		var arc = arcs[i]
		context.strokeStyle = colors[i % colors.length]
		arc.stroke()
	}
}
