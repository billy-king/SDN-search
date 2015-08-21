module.exports = function(app){

  app.get('/', function(req, res){
    res.render('index.jade');
  });
  app.get('/control-list' , function(req, res){
    res.render('control-list.jade');
  })
  app.get('/floodlight', function(req, res){
    res.render('controller/floodlight.jade');
  });
  app.get('/ryu', function(req, res){
    res.render('controller/ryu.jade');
  });
  app.get('/opendaylight', function(req, res){
    res.render('controller/opendaylight.jade');
  });
  app.get('/onos', function(req, res){
    res.render('controller/onos.jade');
  });

}