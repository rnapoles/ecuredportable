/*
String.prototype.trim = function() {
  return this.replace(/^\s+|\s+$/g,"");
}
*/

$(document).ready(function() {

	

    $('a').click(function() {
        link = this.href;
        link = link.trim();
        if(link!=''){
			if(!(/^http:\/\/ecured/).test(link)){
				Page.openExternalUrl(link);
			}	
		}	
		
    });
    
  /*  
    $('.infobox_v2,.infobox_v1,.infobox').css("float","right");
    $('.infobox_v2,.infobox_v1,.infobox').css("border-style","solid");
    $('.infobox_v2,.infobox_v1,.infobox').css("border-width","thin");
    $('.infobox_v2,.infobox_v1,.infobox').css("border-color","#b4bbc8");


   // $('td').css("border-top-color","#ff0000");
   // $('table').css({ borderColor:"cdd6e8" });
    
    
    $(".infobox_v2").mouseover(function()
     {
		$(this).css("border", "solid red 1px");
     });
*/
	/*
    $(".infobox").mouseover(function()
     {
		$(this).css("border", "solid red 1px");
     });*/
     
    //$('.infobox_v2,.infobox_v1,.infobox').remove();

});