var express = require('express');
var app = module.exports = express();
var path = require('path');
var bodyParser = require('body-parser');
var routes = require('./routes/index');
var users = require('./routes/users');
var multer = require('multer');
var jQuery = require('jquery');
var debug = require('debug')('FetalMRI:server');
var http = require('http').Server(app);
var io = require('socket.io')(http);

app.myIO = io;

// view engine setup
app.set('view engine', 'jade');

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(multer({ 
	dest: './uploads/',
	rename: function (fieldname, filename) {
    return filename
  },
  putSingleFilesInArray: true,
  onParseEnd: function (req, next) {
    req.next();
  }
}));

app.use(express.static(path.join(__dirname, 'public')));

app.use('/', routes);
app.use('/users', users);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// --- error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
  app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
      message: err.message,
      error: err
    });
  });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
  res.status(err.status || 500);
  res.render('error', {
    message: err.message,
    error: {}
  });
});
// -----


// io.on('connection', function(socket){
//   console.log('a user connected');
//   socket.on('disconnect', function(){
//     console.log('user disconnected');
//   });
// });



// Event fired every time a new client connects:
io.on('connection', function(socket) {
    console.info('New client connected (id=' + socket.id + ').');
    // When socket disconnects, remove it from the list:
    socket.on('disconnect', function() {
        console.info('Client gone (id=' + socket.id + ').');
    });
});



// ---- enough setup lets get going

/**
 * Normalize a port into a number, string, or false.
 */

function normalizePort(val) {
  var port = parseInt(val, 10);

  if (isNaN(port)) {
    // named pipe
    return val;
  }

  if (port >= 0) {
    // port number
    return port;
  }

  return false;
}


/**
 * Get port from environment and store in Express.
 */

var port = normalizePort(process.env.PORT || '3000');
app.set('port', port);


/**
 * Listen on provided port, on all network interfaces.
 */

/**
 * Event listener for HTTP server "error" event.
 */

function onError(error) {
  if (error.syscall !== 'listen') {
    throw error;
  }

  var bind = typeof port === 'string'
    ? 'Pipe ' + port
    : 'Port ' + port;

  // handle specific listen errors with friendly messages
  switch (error.code) {
    case 'EACCES':
      console.error(bind + ' requires elevated privileges');
      process.exit(1);
      break;
    case 'EADDRINUSE':
      console.error(bind + ' is already in use');
      process.exit(1);
      break;
    default:
      throw error;
  }
}

/**
 * Event listener for HTTP server "listening" event.
 */

function onListening() {
     console.log('listening on *:3000');
}


module.exports = app;

http.listen(port);
http.on('error', onError);
http.on('listening', onListening);

