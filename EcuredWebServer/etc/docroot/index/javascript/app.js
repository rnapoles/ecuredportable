/*
String.prototype.trim = function() {
  return this.replace(/^\s+|\s+$/g,"");
}
*/
UpdateAll = function() {


	$("a[href]").each(function(){
		//alert(this.href);
		linea=this.href;
		if ((/^http:\/\/http/).test(linea)){
			this.href=linea.replace(/^http:\/\/http/g,'http:'); 
			//alert(this.href);
		}		
		this.href=linea.replace(/\/index\//g,'/index/?buscar='); 
		console.log(this.href);
	}); 
	
	
	$("a[href]").each(function()
   { 
		link = this.href;
        link = link.trim();



		if(!(/^(ftp|http|https|gopher|mailto|news|nntp|telnet|wais|file|prospero|aim|webcal):/).test(link)){
			 console.log(this.href);
			 //alert(this.href);
			 //this.href = 'http://ecured/'+this.href;
		}   

		if ((/^http:\/\/ecured\/articles\/all/).test(link)){
			var re = /articles\/all\//gi
			this.href = this.href.replace(re, ""); 
		}
		if(this.title.indexOf("Categoría:")!=-1){
		   temp = encodeURIComponent("Categoría");
		   splitstring = this.href.split(temp);
		   this.href = splitstring[0].concat(temp+":",splitstring[1].substring(1,splitstring[1].length));
		} 
		if(this.title.indexOf("Portal:")!=-1){
		   temp = encodeURIComponent("Portal");
		   splitstring = this.href.split(temp);
		   this.href = splitstring[0].concat(temp+":",splitstring[1].substring(1,splitstring[1].length));
		   if (this.title.indexOf("/")!=-1) {
		   	splitstring = this.href.split("-");
		   	this.href = "".concat(splitstring[0]+"/",splitstring[1]);
		   }
		} 
     
   });


    $('a').click(function(event) {
        link = this.href;
        //alert(link);
        link = link.trim();
        if(link!=''){
        
			
/*			if(MainWindow.IsUsingRemoteServer()){
				
				var IP = MainWindow.GetServerIP();
				var ServerInfo = MainWindow.GetServerInfo();
					ServerInfo = " obj =  " + ServerInfo ;
					
					try {
						ServerInfo = eval(ServerInfo);
					} catch(e) {
						return ;
					}
				
				var WebServer = IP + ':' +ServerInfo.WebServerPort;
				var Exp = "http:\/\/"+WebServer+"\/Archivo";
				var ExpReg = new RegExp(Exp, "i");
				
				if(ExpReg.test(link)){
					this.href='#';
				}		
				
				Exp = "^http:\/\/"+WebServer;
				ExpReg = new RegExp(Exp);
				
				if(!(/^(ftp|http|https|gopher|mailto|news|nntp|telnet|wais|file|prospero|aim|webcal):/).test(link)){
					Page.SearchPage(link);
				} else if(!ExpReg.test(link)){
					try {
						alert(link);
						Page.openExternalUrl(link);
					} catch (e){
					
					}
				}				
					
			}*/
		}	
		
    });
    
	//document.getElementsByClassName("MediaTransformError")[0].innerHTML = ""
	if ( $("#mw-category-media").length ) {
		$("#mw-category-media").css('visibility', 'hidden');
	}
	if ( $(".MediaTransformError").length ) {
	   $(".MediaTransformError")[0].innerHTML = '';
	}

	$('.MediaTransformError').remove();


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

}
