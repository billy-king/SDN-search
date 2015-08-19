var express       = require('express');
var path          = require('path');
var app           = express();


require('./middleware')(app);



// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');


/*
 *static file設定 in project_path/public and project_path/bower_components
 */
app.use(express.static(path.join(__dirname, '/public')));
app.use(express.static(__dirname + '/bower_components'));

app.get('/', function(req, res){
  res.render('index.jade');
});
app.get('/floodlight', function(req, res){
  res.render('floodlight.jade');
});

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handlers

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


module.exports = app;
