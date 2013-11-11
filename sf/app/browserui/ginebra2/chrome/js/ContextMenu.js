

$(document).ready(function() {

  //Get all the LI from the #tabMenu UL
  $('#tabMenu > li').click(function(){

    //perform the actions when it's not selected
    if (!$(this).hasClass('selected')) {

        //remove the selected class from all LI
        $('#tabMenu > li').removeClass('selected');

        //Reassign the LI
        $(this).addClass('selected');

        //Hide all the DIV in .boxBody
        $('.boxBody div').slideUp('1500');

        //Look for the right DIV in boxBody according to the Navigation UL index, therefore, the arrangement is very important.
        $('.boxBody div:eq(' + $('#tabMenu > li').index(this) + ')').slideDown('1500');
    }

  }).mouseover(function() {
    //Add and remove class, Personally I dont think this is the right way to do it, anyone please suggest
    $(this).addClass('mouseover');
    $(this).removeClass('mouseout');

  }).mouseout(function() {

    //Add and remove class
    $(this).addClass('mouseout');
    $(this).removeClass('mouseover');

  });

  //Mouseover effect for menu list.
  $('.boxBody li').click(function(){
    window.snippets.ContextMenuId.hide();
  }).mouseover(function() {
    $(this).css('backgroundColor','#fff');
  }).mouseout(function() {
    $(this).css('backgroundColor','');
  });

});

