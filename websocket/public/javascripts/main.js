$(function(){
  $('.btn-toggle').click(function() {
    $(this).find('.btn').toggleClass('active');

    if ($(this).find('.btn-on').size()>0) {
      $(this).find('.btn-on').toggleClass('btn-success');
    }
    if ($(this).find('.btn-off').size()>0) {
      $(this).find('.btn-off').toggleClass('btn-danger');
    }

    $(this).find('.btn').toggleClass('btn-default');

  });

  $('form').submit(function(){
    alert($(this["options"]).val());
      return false;
  });
})