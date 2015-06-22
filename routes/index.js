var express = require('express');
var router = express.Router();
var fs = require('fs');
var spawn = require('child_process').spawn;
var path = require('path');
var tmp = require('tmp');
var url = require('url');

function startProcessing(req, res)
{
    console.log('startProcessing');

    var dir = tmp.dirSync({ template: './transactions/XXXXXX' });
    console.log("Dir: ", dir.name);
 
    var work = spawn('./recon.sh', [dir.name]);

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
    res.writeHead(200, {"Content-Type": "text/plain"});
    var work=fs.createReadStream("./transactions/" + trans + '/status');
    work.on('close', function(code) {
	console.log("TRANSACTION DONE: $trans ");
	res.end("ALL DONE");
    });
    work.pipe(res);
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
