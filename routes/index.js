var express = require('express');
var router = express.Router();


/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.get('/sliceDrop', function(req, res, next) {
  res.render('sliceDrop');
});

router.get('/ns-viewer', function(req, res, next) {
  res.render('ns-viewer');
});

router.get('/file-upload', function(req, res, next) {
  res.render('file-upload', { title: 'Express' });
});

router.post('/file-upload', function(req, res, next) {
    console.log(req.body);
    console.log(req.files);
});

module.exports = router;
