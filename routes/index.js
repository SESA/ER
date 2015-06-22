var express = require('express');
var router = express.Router();
var fs = require('fs');
var spawn = require('child_process').spawn;
var path = require('path');
var tmp = require('tmp');
var url = require('url');

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
    //    res.redirect('/');
    var urlprefix=reconPrefix(req);

    var body = '<html>'+
	'<head>'+
	'<meta http-equiv="Content-Type" '+
	'content="text/html; charset=UTF-8" />'+
	'</head>'+
	'<body onload="timer=setTimeout(function(){ window.location=\'/sliceDrop/?' + urlprefix + trans + '.nii\';}, 6000)">' +
	'<pre>' +
	'OUTPUT OF RECONSTRUCTION PROCESSING:'	;

    res.writeHead(200, {"Content-Type": "text/html"});
    res.write(body);
    
    var work=fs.createReadStream("./transactions/" + trans + '/status');
    work.on('data', function(chunk){
	console.log("TRANSACTION DATA: " + trans );
	res.write(chunk.toString());
    });
    
    work.on('end', function() {
	console.log("TRANSACTION DONE: " + trans );
	res.write('YOU WILL BE REDIRETED TO RECONSTRUCTION IN 6 seconds...');
	res.end('</pre></body></html>');
    });
}

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
    startProcessing(req, res);
//    res.redirect('foo');
});


router.get(/status/, function(req, res, next) {
    status(req, res, next);
});
	   
module.exports = router;
