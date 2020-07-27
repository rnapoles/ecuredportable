// Última actualización: 03:41 8 feb, 2005 (CET)
//http://es.wikipedia.org/wiki/Usuario:Pilaf/Live_Preview
//wikicode.js

// User options
var wpUserName = wpUserName || 'Wikipedista';
var wpUserSignature = wpUserSignature || wpUserName;
var wpShowImages = wpShowImages || true;

// System options
var wpLanguageCode = 'es';
var wpInterwikiCodes = 'ab|aa|af|ak|sq|als|am|ang|ar|an|arc|hy|roa-rup|as|ast|av|ay|az|bm|ba|eu|be|bn|bh|bi|bs|br|bg|my|ca|ch|ce|chr|chy|ny|zh|zh-tw|zh-cn|cho|cv|kw|co|cr|hr|cs|da|dv|nl|dz|en|eo|et|ee|fo|fj|fi|fr|fy|ff|gl|ka|de|got|el|kl|gn|gu|ht|ha|haw|he|hz|hi|ho|hu|is|io|ig|id|ia|ie|iu|ik|ga|it|ja|jv|kn|kr|csb|ks|kk|km|ki|rw|rn|tlh|kv|kg|ko|kj|ku|ky|lo|la|lv|li|ln|lt|jbo|nds|lg|lb|mk|mg|ms|ml|mt|gv|mi|minnan|mr|mh|zh-min-nan|mo|mn|mus|nah|na|nv|ne|se|no|nn|oc|or|om|pi|fa|pl|pt|pa|ps|qu|ro|rm|ru|sm|sg|sa|sc|gd|sr|sh|st|tn|sn|scn|simple|sd|si|sk|sl|so|st|es|su|sw|ss|sv|tl|ty|tg|ta|tt|te|th|bo|ti|tpi|to|tokipona|ts|tum|tr|tk|tw|uk|ur|ug|uz|ve|vi|vo|wa|cy|wo|xh|ii|yi|yo|za|zu';
var wpBaseArticlePath = '';
var wpMathBasePath = '/math/';
//var wpImageBasePath = 'http://upload.wikimedia.org/wikipedia/' + wpLanguageCode + '/';
var wpImageBasePath = '';
//var wpImageFallbackPath = 'http://upload.wikimedia.org/wikipedia/commons/';
var wpImageFallbackPath = '';
var wpDefaultThumbWidth = 180;
//var wpSkinMagnifyClip = '/skins/common/images/magnify-clip.png';
var wpSkinMagnifyClip = '';
var wpUserNamespace = 'Usuario';
var wpImageNamespace = 'Imagen';
var wpCategoryNamespace = 'Category';

// ¡NO MODIFICAR LO SIGUIENTE!

String.prototype.trim = function(){ return this.replace(/^\s+|\s+$/g,'') }



//var re = /<H([1-6])\s*[^>]*>(.*?)<\/h[1-6]>/i;


var Menu = Array();
var First = 0;

function AppReady(){
	walk($('body'));
	BuidMenu();
	$('img').remove();
    $('.contenedor_imagen').append('<img src="imagenes/ecured.png">');
	 //$('#IdContenedor').remove('img');
    
}


function walk(children , level){
	
	 if (level == null){
	   level = 0;
	 }
	 
	if (typeof children == "undefined" || children.size() === 0) {
		return;
	}
 
	children.each(function(){
		
		var child = $(this);
		
		var obj = child.get(0);    
		var str = obj.tagName;
		
		var found = false;
		
		
		if (htag = str.match(/h([1-6])/i)){
		
			   	
			   var htexto = obj.innerHTML;
			   obj.innerHTML = '<span id="'+htexto.trim()+'" class="useScroll">'+htexto+'</span>';

			   if(First==0) {
			    obj.id="HFirst"; 
			    First++;
			   }			   
			    
			   MenuObj = new Object();
			   MenuObj.texto = htexto;
			   MenuObj.level = htag[1]; 
			   Menu.push(MenuObj); 
			   
			   
			   found = true;	
		       str = str.substr(1,1);
			   //wikihelper.debug(htag[1] +' '+obj.innerHTML);
		}			
		
		if (child.children().size() > 0) {
			if(found) level++;
			walk(child.children(),level);
			if(found) level--;
		}

	
		
	});
}

function BuidMenu(){
	 var size = Menu.length;
	 
	 if(size==0)  {
		$('.contenedor_derecho2').remove();
		return;
	 }
	 
	 //for(i=size;i=0;i--)
	 var toc = $('#toc_indice');
	 var InsertText = '<ol>';
	 var obj = null;
	 
	 for(i in Menu){
	        var texto = Menu[i].texto;
	        if(Menu[i].first){
				obj = Menu[i].firstObj; 
	        }
			//wikihelper.debug("<a href=#"+texto.trim()+">"+texto+"</a>");
			//toc.append("<li><a href=#"+texto.trim()+">"+texto+"</a></li>\n");
			InsertText += "<li><a href=\"#"+texto.trim()+"\">"+texto+"</a></li>\n";
			
	 }
	
	 InsertText+='</ol>';		
	 toc.append(InsertText);
	 //$('#HFirst').siblings().get(0).insertBefore
	 $('.contenedor_derecho2').insertBefore('#HFirst');
	 
  
    
	  //.appendTo(.parent());
	 
}


function wiki2html(str) {


      /*

      str =  str.replace(/\{\{((?:.|\n)*?)\}\}/mg,function (mstr,p1,p2,totalstr){
        return '';      
      });
      
      
            
      str =  str.replace(/\{\{(.*?)\|(.*?)\|(.*?)\}\}/mg,function (mstr,p1,p2,p3,totalstr){
        return '';      
      });


      str =  str.replace(/\{\{(.*?)\}\}/mg,function (mstr,p1,totalstr){
        return '';      
      });
      
      */
      


		str =  str.replace(/\{\{(.+?)\}\}/gm,function (mstr,p1,totalstr){
			console.log(mstr);
			//wikihelper.debug(mstr); 
			return '';
		})
		        
	
		.replace(/\{\{\{\{(.+?)\}\}\}\}/,function (mstr,p1,totalstr){
			//wikihelper.debug(mstr); 
			console.log(mstr);
			return '\n';
		})
		
		.replace(/\{\{([^\}]+)\}\}/gm,function (mstr,p1,totalstr){
			//wikihelper.debug(mstr); 
			console.log(mstr);
			return '\n';
		}) 


        .replace(/\[\[Image:(.*?)\]\]/g, function (m, l) { // internal link or image
            var p = l.split(/\|/);
            var link = p.shift();
			return ' ';
        })
        
		
		/*
        .replace(/\[\[(.*?)\]\]/g, function (m, l) { // internal link or image
            var p = l.split(/\|/);
            var link = p.shift();

            if (link.match(/^Image:(.*)/)) {
                // no support for images - since it looks up the source from the wiki db :-(
                return m;
            }
        });*/   		
				
		
    /*
      str =  str.replace(/\{\{([^!|}]+)((\|\+.*)?)((.*\n)+?)\}\}/gm,function (mstr,p1,p2,p3,p4,totalstr)  {
			
              var tabla = '<table>';
                //console.log(p1);  
                wikihelper.debug(mstr); 
              var elementos = p4.split(/\n\|/);   
              for(i in elementos){
                  var s = elementos[i];
                  if(s.match(/^\|/)) {
                    s = s.substr(1);
                  }
                  
                  var Part = s.split('=');
                  //console.log(Part[0]);   
                  //console.log(Part[1]);   
              }
              //return 'Plantilla:'+p1;   
              return '';   
      })*/





	
    str = strip_cr(str);
	    
    var w = new WikiCode();
    w.lines = str.split(/\n/);
    w.parse();
    
    str = w.html;
    
    
	str	=str.replace(/<H([1-6])(.*?'.'>)(.*?)<\/H[1-6] *>/gi,function (mstr,p1,p2,p3,totalstr){
			//wikihelper.debug("mstr:"+mstr+"\n"); 
			//wikihelper.debug("p1:"+p1+"\n"); 
			//wikihelper.debug("p2:"+p2+"\n"); 
			//wikihelper.debug("p3:"+p3+"\n"); 
			//wikihelper.debug("totalstr:"+totalstr+"\n"); 
			return '\n';
		}) ;    
    
    
    return str;
};
var wpSignature = '[[' + wpUserNamespace + ':' + wpUserName + '|' + wpUserSignature + ']]';
var wpBlockImage = new RegExp('^\\[\\[' + wpImageNamespace + ':.*?\\|.*?(?:frame|thumbnail|thumb|none|right|left|center)', 'i');

function WikiCode() {
    this.lines = new Array;
    this.html = new String;
    this._endline = function (str) {
        this.html += str;
        this.lines.shift();
    };
    this.parse = function () {
        var p = false;
        do {
            if (h_match = this.lines[0].match(/^(={1,6})(.*)\1(.*)$/)) {
                p = false;
                this._endline('<h' + h_match[1].length + '>' + _parse_inline_nowiki(h_match[2]) + '</h' + h_match[1].length + '>' + h_match[3]);
            } else if (this.lines[0].match(/^[*#:;]/)) {
                p = false;
                this._parse_list();
            } else if (this.lines[0].charAt(0) == ' ') {
                p = false;
                this._parse_pre();
            } else if (this.lines[0].substr(0, 2) == '{|') {
                p = false;
                this._parse_table();
            } else if (this.lines[0].match(/^----+$/)) {
                p = false;
                this._endline('<hr/>');
            } else if (this.lines[0].match(wpBlockImage)) {
                p = false;
                this._parse_block_image();
            } else {
                if (this.lines[0] == '') {
                    if (p = (this.lines.length > 1 && this.lines[1] == '')) {
                        this._endline('<p><br />');
                    }
                } else {
                    if (!p) {
                        this.html += '<p>';
                        p = true;
                    }
                    this.html += _parse_inline_nowiki(this.lines[0]) + ' ';
                }
                this.lines.shift();
            }
        } while (this.lines.length);
    };
    this._parse_list = function () {
        var prev = new String;
        var l_match, imatch, dt_match;
        while (this.lines.length && (l_match = this.lines[0].match(/^([*#:;]+)(.*)$/))) {
            this.lines.shift();
            imatch = str_imatch(prev, l_match[1]);
            for (var i = prev.length - 1; i >= imatch; i--) {
                if (prev.charAt(i) == '*') {
                    this.html += '</ul>';
                } else if (prev.charAt(i) == '#') {
                    this.html += '</ol>'
                } else {
                    this.html += '</d' + ((prev.charAt(i) == ';') ? 't' : 'd') + '>';
                    switch (l_match[1].charAt(i)) {
                    case '':
                    case '*':
                    case '#':
                        this.html += '</dl>';
                    }
                }
            }
            for (var i = imatch; i < l_match[1].length; i++) {
                if (l_match[1].charAt(i) == '*') {
                    this.html += '<ul>';
                } else if (l_match[1].charAt(i) == '#') {
                    this.html += '<ol>';
                } else {
                    switch (prev.charAt(i)) {
                    case '':
                    case '*':
                    case '#':
                        this.html += '<dl>';
                    }
                    this.html += '<d' + ((l_match[1].charAt(i) == ';') ? 't' : 'd') + '>';
                }
            }
            switch (l_match[1].charAt(l_match[1].length - 1)) {
            case '*':
            case '#':
                this.html += '<li>' + _parse_inline_nowiki(l_match[2]);
                break;
            case ';':
                if (dt_match = l_match[2].match(/(.*?) (:.*?)$/)) {
                    this.html += _parse_inline_nowiki(dt_match[1]);
                    this.lines.unshift(dt_match[2]);
                    break;
                }
            case ':':
                this.html += _parse_inline_nowiki(l_match[2]);
            }
            prev = l_match[1];
        }
        for (i = prev.length - 1; i >= 0; i--) {
            if (prev.charAt(i) == '*') {
                this.html += '</ul>';
            } else if (prev.charAt(i) == '#') {
                this.html += '</ol>';
            } else {
                this.html += '</d' + ((prev.charAt(i) == ';') ? 't' : 'd') + '></dl>';
            }
        }
    };
    this._parse_table = function () {
        var table_match;
        if (table_match = this.lines[0].match(/^\{\|( .*)$/)) {
            this._endline('<table' + table_match[1] + '>');
        } else this._endline('<table>');
        do {
            if (this.lines[0].charAt(0) == '|') {
                switch (this.lines[0].charAt(1)) {
                case '}':
                    this._endline('</table>');
                    return;
                case '-':
                    this._endline('<tr ' + this.lines[0].match(/\|-*(.*)/)[1] + '>');
                    break;
                default:
                    this._parse_table_data();
                }
            } else if (this.lines[0].charAt(0) == '!') {
                this._parse_table_data();
            } else {
                this.lines.shift();
            }
        } while (this.lines.length)
    };
    this._parse_table_data = function () {
        var td_match, td_line;
        td_match = this.lines.shift().match(/^(\|\+|\||!)((?:([^[|]*?)\|(?!\|))?(.*))$/);
        if (td_match[1] == '|+') {
            this.html += '<caption';
        } else {
            this.html += '<t' + ((td_match[1] == '|') ? 'd' : 'h');
        }
        if (typeof td_match[3] != 'undefined') {
            this.html += ' ' + td_match[3];
            td_line = td_match[4].split('||');
        } else {
            td_line = td_match[2].split('||');
        }
        this.html += '>';
        while (td_line.length > 1) {
            this.lines.unshift(td_match[1] + td_line.pop());
        }
        this.html += _parse_inline_nowiki(td_line[0]);
        var td = new WikiCode;
        var table_count = 0;
        while (this.lines.length) {
            if (this.lines[0].charAt(0) == '|') {
                if (table_count == 0) break;
                else if (this.lines[0].charAt(1) == '}') table_count--;
            } else if (this.lines[0].charAt(0) == '!' && table_count == 0) {
                break;
            } else if (this.lines[0].substr(0, 2) == '{|') table_count++;
            td.lines.push(this.lines.shift());
        }
        if (td.lines.length) {
            td.parse();
        }
        this.html += td.html;
    };
    this._parse_pre = function () {
        this.html += '<pre style="white-space:pre-wrap;">';
        do {
            this._endline(_parse_inline_nowiki(this.lines[0].substring(1, this.lines[0].length)) + "\n");
        } while (this.lines.length && this.lines[0].charAt(0) == ' ');
        this.html += '</pre>';
    };
    this._parse_block_image = function () {
        this.html += _parse_image(this.lines.shift());
    };
};

function _parse_image(str) {
    var attr = str.substring(wpImageNamespace.length + 3, str.length - 2).split(/\s*\|\s*/);
    var filename = attr[0];
    var caption = attr[attr.length - 1];
    var width, w_match;
    var thumb = false;
    var frame = false;
    var center = false;
    var align = '';
    var html = '';
    do {
        if (w_match = attr[0].match(/^(\d*)px$/)) {
            width = w_match[1];
        } else switch (attr[0]) {
        case 'thumb':
        case 'thumbnail':
            thumb = true;
        case 'frame':
            frame = true;
            break;
        case 'none':
        case 'right':
        case 'left':
            center = false;
            align = attr[0];
            break;
        case 'center':
            center = true;
            align = 'none';
        }
        attr.shift();
    } while (attr.length);
    if (frame) {
        if (align == '') align = 'right';
        html += "<div class='thumb t" + align + "'>";
        if (thumb) {
            if (!width) width = wpDefaultThumbWidth;
            html += "<div style='width:" + (2 + parseInt(width)) + "px;'>";
            html += _make_image(filename, caption, width);
            html += "<div class='thumbcaption'><div class='magnify' style='float:right'><a href='" + wpBaseArticlePath + wpImageNamespace + ':' + filename + "' class='internal' title='Enlarge'><img src='" + wpSkinMagnifyClip + "' /></a></div>" + _parse_inline_nowiki(caption) + "</div>";
        } else {
            html += '<div>';
            html += _make_image(filename, caption);
            html += "<div class='thumbcaption'>" + _parse_inline_nowiki(caption) + "</div>";
        }
        html += '</div></div>';
    } else if (align != '') {
        html += "<div class='float" + align + "'><span>" + _make_image(filename, caption, width) + "</span></div>";
    } else {
        return _make_image(filename, caption, width);
    }
    if (center) {
        return "<div class='center'>" + html + '</div>';
    } else {
        return html;
    }
};
this._parse_inline_nowiki = function (str) {
    var start, lastend = 0
    var substart = 0,
        nestlev = 0,
        open, close, subloop;
    var html = '';
    while (-1 != (start = str.indexOf('<' + 'nowiki>', substart))) {
        html += _parse_inline_wiki(str.substring(lastend, start));
        start += 8;
        substart = start;
        subloop = true;
        do {
            open = str.indexOf('<' + 'nowiki>', substart);
            close = str.indexOf('</nowiki>', substart);
            if (close <= open || open == -1) {
                if (close == -1) {
                    return html + html_entities(str.substr(start));
                }
                substart = close + 9;
                if (nestlev) {
                    nestlev--;
                } else {
                    lastend = substart;
                    html += html_entities(str.substring(start, lastend - 9));
                    subloop = false;
                }
            } else {
                substart = open + 8;
                nestlev++;
            }
        } while (subloop);
    }
    return html + _parse_inline_wiki(str.substr(lastend));
};

function _make_image(filename, caption, width) {
    filename = filename.charAt(0).toUpperCase() + filename.substr(1);
    filename = filename.replace(/ /g, '_');
    var source =  filename;
    var img;
    if (wpShowImages) {
        if (width) {
            width = "width='" + width + "px'";
        }
        img = "<img onerror='this.onerror=null;this.src=\"" + wpImageFallbackPath + source + "\";' src='" + wpImageBasePath + source + "' alt='" + caption + "' " + width + "/>";
    } else {
        img = wpImageNamespace + ':' + filename + " <em style='color:red;'>(images disabled)</em>";
    }
    caption = _strip_inline_wiki(caption);
    return "<a class='image' title='" + caption + "' href='" + wpBaseArticlePath + wpImageNamespace + ':' + filename + "'>" + img + "</a>";
};

function _parse_inline_images(str) {
    var start, substart = 0,
        nestlev = 0;
    var loop, close, open, wiki, html;
    while (-1 != (start = str.indexOf('[[', substart))) {
        if (str.substr(start + 2).match(RegExp('^' + wpImageNamespace + ':', 'i'))) {
            loop = true;
            substart = start;
            do {
                substart += 2;
                close = str.indexOf(']]', substart);
                open = str.indexOf('[[', substart);
                if (close <= open || open == -1) {
                    if (close == -1) return str;
                    substart = close;
                    if (nestlev) {
                        nestlev--;
                    } else {
                        wiki = str.substring(start, close + 2);
                        html = _parse_image(wiki);
                        str = str.replace(wiki, html);
                        substart = start + html.length;
                        loop = false;
                    }
                } else {
                    substart = open;
                    nestlev++;
                }
            } while (loop);
        } else {
            break;
        }
    }
    return str;
};

function _parse_inline_wiki(str) {
    var aux_match, math_md5;
    str = _parse_inline_images(str);
    while (aux_match = str.match(/<(?:)math>(.*?)<\/math>/i)) {
        math_md5 = wikihelper.convertToBase64(aux_match[1]);
        str = str.replace(aux_match[0], "<img src='" + wpMathBasePath + math_md5 + '.png' + "' />");
    }
    return str.replace(/'''''(.*?)''(.*?)'''/g, '<strong><em>$1</em>$2</strong>').replace(/'''''(.*?)'''(.*?)''/g, '<em><strong>$1</strong>$2</em>')
		   .replace(/'''(.*?)''(.*?)'''''/g, '<strong>$1<em>$2</em></strong>')
		   .replace(/'''(.*?)'''/g, '<strong>$1</strong>')
		   .replace(/''(.*?)''/g, '<em>$1</em>')
		   .replace(/~{5}(?!~)/g, Date())
		   .replace(/~{4}(?!~)/g, wpSignature + ' ' + Date())
		   .replace(/~{3}(?!~)/g, wpSignature)
		   //.replace(RegExp('\\[\\[:((?:' + wpCategoryNamespace + '|' + wpInterwikiCodes + '):.*?)\\]\\]', 'gi'), "<a href='" + wpBaseArticlePath + "$1'>$1</a>")
		   .replace(/\[\[([^\]]*?:)[^\]]*\]\]/gm, "")
		   .replace(RegExp('\\[\\[:((?:' + wpCategoryNamespace + '|' + wpInterwikiCodes + '):.*?)\\]\\]', 'gi'), "")
		   .replace(RegExp('\\[\\[(?:' + wpCategoryNamespace + '|' + wpInterwikiCodes + '):.*?\\]\\]', 'gi'), '')
		   .replace(/\[\[([^|]*?)\]\](\w*)/g, "<a href='" + wpBaseArticlePath + "$1'>$1$2</a>")
		   .replace(/\[\[(.*?)\|([^\]]+?)\]\](\w*)/g, "<a href='" + wpBaseArticlePath + "$1'>$2$3</a>")
		   .replace(/\[\[([^\]]*?:)?(.*?)( *\(.*?\))?\|\]\]/g, "<a href='" + wpBaseArticlePath + "$1$2$3'>$2</a>")
		   .replace(/\[(http|news|ftp|mailto|gopher|irc):(\/*)([^\]]*?) (.*?)\]/g, "<a href='$1:$2$3'>$4</a>")
		   .replace(/\[http:\/\/(.*?)\]/g, "<a href='http://$1'>[#]</a>").replace(/\[(news|ftp|mailto|gopher|irc):(\/*)(.*?)\]/g, "<a href='$1:$2$3'>$1:$2$3</a>")
		   .replace(/(^| )(http|news|ftp|mailto|gopher|irc):(\/*)([^ $]*)/g, "$1<a href='$2:$3$4'>$2:$3$4</a>")
		   .replace('__NOTOC__', '')
		   .replace('__NOEDITSECTION__', '');
};

function _strip_inline_wiki(str) {
    return str.replace(/\[\[[^\]]*\|(.*?)\]\]/g, '$1').replace(/\[\[(.*?)\]\]/g, '$1').replace(/''(.*?)''/g, '$1');
};

function max(a, b) {
    if (a > b) return a;
    else return b;
};

function min(a, b) {
    if (a < b) return a;
    else return b;
};

function str_imatch(str_a, str_b) {
    var lim = min(str_a.length, str_b.length);
    for (var i = 0; i < lim; i++) {
        if (str_a.charAt(i) != str_b.charAt(i)) return i;
    }
    return i;
};

function strip_cr(str) {
    return str.replace(/\n\r/g, "\n").replace(/\r/g, '');
};

function html_entities(str) {
    return str.replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;");
};
var chrsz = 8;
var hex_tab = "0123456789abcdef";






