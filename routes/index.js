var express = require('express');
var router = express.Router();
var fs = require('fs');
var spawn = require('child_process').spawn;
var path = require('path');
var tmp = require('tmp');
var url = require('url');
var util = require('util');


function reconPrefix(req){
    return 'http://' + req.get('host') + '/recon/';
}

function startProcessing(req, res)
{
    console.log('startProcessing: req: ' + req.get('host') +
		'host: ' + req.hostname + 
		' port: ' + req.app.settings.port +
		' url: ' + req.url);

    var dir = tmp.dirSync({ template: './transactions/XXXXXX' });
    var files = null;
    var urlprefix=reconPrefix(req);
    
    console.log("Dir: ", dir.name);
    req.files.slice.forEach(function (element, index, array) {
	if (files == null) files = element.path;
	else files = files + ' ' + element.path;
    });
    console.log("files: ", files);
    
    var work = spawn('./recon.sh', [dir.name, urlprefix, files]);

    work.on('error', function (er) {
	console.log("error" + er);
    });
    
    work.stderr.setEncoding('utf8');
    work.stdout.setEncoding('utf8');

    work.stdout.on('data', function (data) {
	data=data.trim();
	console.log('stdout: ' + data);
    });
    
    work.stderr.on('data', function (data) {
	data= data.trim();
	console.log('stderr: ' + data);
    });
    
    work.on('close', function (code) {
	console.log('work closed/exited with code ' + code);
    });

    res.send(path.basename(dir.name));
}

function status(req, res, next)
{
    var trans = path.basename(url.parse(req.url).pathname);
    console.log('status: ' + trans);
    var urlprefix=reconPrefix(req);

    var io = req.app.myIO;

    console.log('io: ' + io);
    
    var work=fs.createReadStream("./transactions/" + trans + '/status');
    work.on('data', function(chunk){
	var data = chunk.toString();
	console.log("TRANSACTION DATA: " + trans + ' : ' + data);
	io.emit('log', data);
    });
    
    work.on('end', function() {
	var url = '/sliceDrop/?' + urlprefix + trans + '.nii';
	console.log("TRANSACTION DONE: " + trans );
	io.emit('ctl', url);
    });
}

/* GET home page. */
router.get('/', function(req, res, next) {
    	res.render('file-upload', { title: 'Elastic Reconstruction' });
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
	res.render('file-upload', { title: 'Elastic Reconstruction' });
});

router.post('/file-upload', function(req, res, next) {
    console.log(req.files);
    startProcessing(req, res);
});


router.get(/status/, function(req, res, next) {
    res.render('status', { title: 'Elastic Reconstruction Status' });
    status(req, res, next);
});
	   
module.exports = router;
