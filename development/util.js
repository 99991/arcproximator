function swap(values, i, j){
	var temp = values[i]
	values[i] = values[j]
	values[j] = temp
}

function shuffle(values){
	for (var i = values.length - 1; i > 0; i--){
		swap(values, i, Math.random()*(i+1)|0)
	}
}

function assert(shouldBeTrue, message){
	if (!shouldBeTrue){
		console.log(message)
	}
}

function append(values, moreValues){
	for (var i = 0; i < moreValues.length; i++){
		values.push(moreValues[i])
	}
}

function sortByKey(values, keyFunc){
	var n = values.length;
	
	var keyedValues = new Array(n)
	for (var i = 0; i < n; i++){
		var value = values[i]
		var key = keyFunc(value)
		keyedValues[i] = [key, value]
	}
	
	keyedValues.sort(function(a, b){
		return cmp(a[0], b[0])
	})
	
	for (var i = 0; i < n; i++){
		values[i] = keyedValues[i][1]
	}
}

function cmp(a, b){
	if (a < b) return -1;
	if (a > b) return +1
	return 0
}

function almostEqual(a, b, eps){
	if (eps === undefined) eps = 1e-10;
	assert(typeof(a) == "number", a + " is not a number")
	assert(typeof(b) == "number", b + " is not a number")
	return Math.abs(a - b) < eps;
}

function almostEqualPoints(a, b, eps){
	assert(Array.isArray(a), a + " is not an array")
	assert(Array.isArray(b), b + " is not an array")
	return almostEqual(a[0], b[0], eps) && almostEqual(a[1], b[1], eps)
}

function filter(f, values){
	var newValues = []
	var n = values.length
	for (var i = 0; i < n; i++){
		var value = values[i]
		if (f(value)) newValues.push(value)
	}
	return newValues
}

function identity(value){
	return value
}

/*
function makePairs(values){
	var n = values.length
	var a = values[n-1]
	var results = new Array(n)
	for (var i = 0; i < n; i++){
		var b = values[i]
		results[i] = [a, b]
		a = b
	}
	return results
}
*/
