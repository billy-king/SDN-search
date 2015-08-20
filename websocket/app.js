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
/*
 *Routing setting
 */
require('./controller')(app);
/*
 *ErrorHandler
 */
require('./errorHandler')(app);

module.exports = app;
