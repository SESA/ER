var express = require('express');
var router = express.Router();
var shelljs = require('shelljs');
var fs = require('fs');

/* GET home page. */
router.get('/', function(req, res, next) {
	res.render('index', { title: 'Express' });
});

router.get('/sliceDrop', function(req, res, next) {
	var view = fs.open('./uploads/10_3T_nody_001.nii.gz' , r, callback);
	console.log(view);
	res.render('slicedrop', {view: view});
});

router.get('/users', function(req, res, next) {
	var view = fs.open('./uploads/10_3T_nody_001.nii.gz' , 'r');
	console.log(fs.read(view));
	res.render('index', {view: view});
});

router.get('/file-upload', function(req, res, next) {
	res.render('file-upload', { title: 'Dropzone' });
});

router.post('/file-upload', function(req, res, next) {
	console.log(req.files);
	res.send('test');
});

module.exports = router;
