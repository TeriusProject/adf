var factory = require('./adflib.js');

factory().then((instance) => {
	console.log(instance._get_version());
	console.log(instance._get_adf());
});
