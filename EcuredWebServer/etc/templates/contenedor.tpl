<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

    <link rel="stylesheet" type="text/css" href="app.css">
    
    <script type="text/javascript" src="javascript/jquery-latest.js"></script>
    <script type="text/javascript" src="javascript/monobook.js"></script>
    <script type="text/javascript" src="javascript/app.js"></script>

		<link rel="stylesheet" href="css/common/shared.css" type="text/css" media="screen">
		<link rel="stylesheet" href="css/common/commonPrint.css" type="text/css" media="print">
		<link rel="stylesheet" href="css/common/Common.css" type="text/css">
		

		<link rel="stylesheet" href="css/ecured/ecured.css" type="text/css" media="screen">
		<link rel="stylesheet" href="css/ecured/print.css" type="text/css" media="print">
		<link rel="stylesheet" href="css/ecured/main.css" type="text/css" media="screen">

    <script type="text/javascript">
      $(document).ready(function() {
        
        {wikicode}
        
        if(wikicode){
            var text = $("#IdContenedor").html();
            text = wiki2html(text);
            $("#IdContenedor").html(text);
            AppReady();
        
        }else{
            $('.contenedor_derecho2').remove();
        }

        
        $('.thumbinner').remove();
        $('.editsection').remove();
        $('.floatright').remove();
        $('.floatleft').remove();
        $('form').remove();
        
        $("pre").each(function(index) {
          //alert(index + ': ' + $(this).text());
          if($(this).text().trim()==''){
            $(this).remove();
          }
        });
         
        UpdateAll();  
        
        /*$("a[href]").each(function()
		{ 
			alert(this.href);
		});*/
                      
      });
    </script>
   
    <title>{title}</title>
    
  </head>
  <body>
    <center>
    <div class="contenedor_general">   
      <div class="contenedor_cabecera">      
        <div class="contenedor_imagen">
          <img src="imagenes/ecured.png"/>
        </div>      
        <div class="contenedor_barra">           
          <div class="contenedor_barra_texto">EcuRed: Enciclopedia cubana
          </div>      
        </div>          
      </div>  
      <div class="contenedor_principal">    
        <div class="contenedor_derecho2">  
          <div class="contenedor_derecho_izq2">            
            
			<!--
			<div class="plantilla_general_texto">El objeto es la 
              <em>Tierra
              </em> como morada de la especie humana, y el   sentido interroga la relación de interdependencia entre la humanidad y   su hábitat. La perspectiva &quot;ecuménica&quot; está desigualmente explícita en   las obras de geografía humana. Está explicitada cuando el autor tiende a   dar a su propósito un valor ético u ontológico, y ambiciona refundir el   proyecto de la geografía humana, volviendo a los orígenes y subrayando   la novedad. 
            </div>   
  
		    -->
			 
				
            <div class="contenedor_derecho_noticias2">
              <div class="contenedor_derecho_noticias_elementos_cabecera2">                                     
                <div class="contenedor_derecho_noticias_cabecera2">                    
                  <div class="contenedor_derecho_noticias_cabecera_texto2">Contenido
                  </div>                  
                </div>          
              </div>                
              <div id="toc_indice" class="contenedor_derecho_noticias_cuerpo2">     
              <!--                
                <ol>                       
                  <li>
                  <a href="#">1 elemento1</a>
                  </li>                       
                  <li>
                  <a href="#">2 elemento2</a>                           
                  <ol>                             
                    <li>
                    <a href="#">2.1 elemento1</a>
                    </li>                             
                    <li>
                    <a href="#">2.2 elemento1</a>
                    </li>                           
                  </ol>                       
                  </li>                       
                  <li>
                  <a href="#">3 elemento3</a>
                  </li>                     
                </ol>                
                -->
              </div>            
            </div>          
          </div> 
			
          <!-- <div class="contenedor_derecho_derecho2">            
            <div class="contenedor_derecho_derecho_noticias2">     
              <div class="contenedor_derecho_derecho_noticias_elementos_cabecera2">                      
                <div class="contenedor_derecho_derecho_noticias_cabecera2">                   
                  <div class="contenedor_derecho_derecho_noticias_cabecera_texto2">Titulo del bloque
                  </div>         
                </div>              
              </div>
			 
              <div class="contenedor_derecho_derecho_noticias_cuerpo2">               
                <img src="imagenes/250px-Mapa_provincias_cubanas.jpg" width="230" height="88"/>                 
                <table width="200" border="0">  
                  <tr>    <td>dfdfsdf</td>    <td>dfsdfdsf</td>  
                  </tr>  
                  <tr>    <td>dsfsdfs</td>    <td>sdfsdfsd</td>  
                  </tr>  
                  <tr>    <td>dfdsfds</td>    <td>rttret</td>  
                  </tr>  
                  <tr>    <td>fdfghg</td>    <td>435ttr</td>  
                  </tr>
                </table>                                   
              </div>   

			 
            </div>          
          </div>    -->
		  
        </div>          
        <div class="contenedor_plantilla_general">
        <br/>                                       
        <br/>          
        	<div id = "IdContenedor" class="textos_normales"> 
              {texto}
          </div>
        </div>            
      </div>       

	  <div class="contenedor_pie_pagina">      
        <div class="contenedor_pie_pagina_firma">
        Centro de Desarrollo Territorial Holgu&iacute;n - UCI&nbsp;&nbsp;<br/>
        Ecured Portable v{version} server&nbsp;&nbsp;<br/>
        2011-2012&nbsp;&nbsp;<br/>
        </div>
      </div>


    </div>
  </center>
  </body>
</html>