var express = require('express');
var router = express.Router();
var fs = require('fs');
var spawn = require('child_process').spawn;
var path = require('path');
var tmp = require('tmp');
var url = require('url');
var util = require('util');

var transactions = {};

function reconPrefix(req){
    return 'http://' + req.get('host') + '/recon/';
}

function startProcessing(req, res)
{
    var dir = tmp.dirSync({ template: './transactions/XXXXXX' });
    var tranid = path.basename(dir.name);
    var ts = { id: tranid, path: dir };
    var files = null;
    var urlprefix = reconPrefix(req);

    console.log("Dir: ", dir.name);
	
    //req.files.forEach(function (element, index, array) {
    //    if (files == null) 
    //        files = element.path;
    //    else 
    //        files = files + ' ' + element.path;
    //});
    //ts.files = files;

    ts.data = '';

    ts.ioNSP = req.app.myIO.of('/' + tranid);
    console.log('created: ts.ioNSP /' + tranid + ':' + ts.ioNSP);
    ts.ioNSP.on('connection', function (socket) {
	console.log('ioNSP: ' + tranid +': Connection');
	if (tranid in transactions) {
	    ts.ioNSP.emit('log', ts.data);
	    socket.on('disconnect', function() {
		console.info('ioNSP: '+ tranid + ': Disconnect Client gone (id=' + socket.id + ').');
	    });
	} else {
	    // race condition handling... status page rendered but transaction terminated and deleted before socket.io
	    // request -- there is still a bug here as the ts object has been deleted... but since
	    // I have no clue about how js memory allocation works I will assume I am ok
	    // till we see the failure ;-)
	    console.log('ioNSP: ' + tranid + ': ERROR');
	    ts.ioNSP.emit('ctl', '/slicedrop/?' + urlprefix + tranid + '.nii');
	}
    });
    
    transactions[tranid] = ts;
    
    //ts.work = spawn('./recon.sh', [dir.name, urlprefix, files]);
    ts.work = spawn('./test.sh');

    ts.work.on('error', function (er) {
	delete transactions[tranid];
	console.log("error" + er);
    });

    
    ts.work.stderr.setEncoding('utf8');
    ts.work.stdout.setEncoding('utf8');

    ts.work.stdout.on('data', function (data) {
        ts.data = ts.data + data;
	ts.ioNSP.emit('log', data);
	data=data.trim();
	console.log('stdout: ' + data);
    });
    
    ts.work.stderr.on('data', function (data) {
	ts.data = ts.data + data;
	ts.ioNSP.emit('log', data);
	data= data.trim();
	console.log('stderr: ' + data);
    });
    
    ts.work.on('close', function (code) {
	console.log("TRANSACTION DONE: " + tranid );
	ts.ioNSP.emit('ctl', '/slicedrop/?' + urlprefix + tranid + '.nii');
	delete transactions[tranid];
	console.log('work closed/exited with code ' + code);
    });

	res.render('status', { title: 'Elastic Reconstruction Status', id: tranid, port: req.app.settings.port });
}

/* GET home page. */
router.get('/', function(req, res, next) {
    	res.render('file-upload', { title: 'Elastic Reconstruction' });
});

router.get('/file-upload', function(req, res, next) {
	res.render('file-upload', { title: 'Elastic Reconstruction' });
});

router.post('/file-upload', function(req, res, next) {
    console.log(req.files);
    res.send();
});

router.get(/status/, function(req, res, next) {
    var tranid = path.basename(url.parse(req.url).pathname);
    if (tranid in transactions) {
	res.render('status', { title: 'Elastic Reconstruction Status', id: tranid, port: req.app.settings.port });
    } else {
	res.writeHead(404, {"Content-Type": "text/html"});
	res.write("404 Not found");
	res.end();
    }
});

router.post('/start-recon', function(req, res, next) {
    startProcessing(req, res);
})
	   
module.exports = router;
