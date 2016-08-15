var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");
var colors = [ "#00aaff", "#8800ff", "#ff0022", "#ccff00", "#00ffee", "#ff00ee", "#ff6600", "#00ff00", "#0066ff" ]
var mouse = [0, 0]

function drawCircle(p, radius, fill){
	context.beginPath()
	context.arc(p[0], p[1], radius, 0, Math.PI*2)
	if (fill) context.fill()
	else context.stroke()
}

function drawPolygon(points, fill){
	var a = points[0]
	context.beginPath()
	context.moveTo(a[0], a[1])
	for (var i = 1; i < points.length; i++){
		var b = points[i]
		context.lineTo(b[0], b[1])
	}
	context.closePath()
	if (fill) context.fill()
	else context.stroke()
}

function write(text, p){
	context.save()
	context.resetTransform()
	context.fillText(text, p[0], canvas.height - p[1])
	context.restore()
}
