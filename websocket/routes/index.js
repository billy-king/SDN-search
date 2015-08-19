var express = require('express');
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});
/* GET users listing. */
router.get('/floodlight', function(req, res, next) {
  res.render('floodlight.jade');
});

module.exports = router;
