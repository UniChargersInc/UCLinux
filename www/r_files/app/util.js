// extend Number object with methods for converting degrees/radians
Number.prototype.toRad = function () { return this * Math.PI / 180; }
Number.prototype.toDeg = function () { return this * 180 / Math.PI; }
//=====================================Dispatch=============================================
/** http://stackoverflow.com/questions/105034/create-guid-uuid-in-javascript
 * Generates a GUID string.
 * @returns {String} The generated GUID.
 * @example af8a8416-6e18-a307-bd9c-f2c947bbb3aa
 * @author Slavik Meltser (slavik@meltser.info).
 * @link http://slavik.meltser.info/?p=142
 */
function guid() {
    function _p8(s) {
        var p = (Math.random().toString(16)+"000000000").substr(2,8);
        return s ? "-" + p.substr(0,4) + "-" + p.substr(4,4) : p ;
    }
    return _p8() + _p8(true) + _p8(true) + _p8();
}
//-------------------------------------------------------------------------------
 /*
     * Date Format 1.2.3
     * (c) 2007-2009 Steven Levithan <stevenlevithan.com>
     * MIT license
     *
     * Includes enhancements by Scott Trenda <scott.trenda.net>
     * and Kris Kowal <cixar.com/~kris.kowal/>
     *
     * Accepts a date, a mask, or a date and a mask.
     * Returns a formatted version of the given date.
     * The date defaults to the current date/time.
     * The mask defaults to dateFormat.masks.default.
     */

    var dateFormat = function () {
        var    token = /d{1,4}|m{1,4}|yy(?:yy)?|([HhMsTt])\1?|[LloSZ]|"[^"]*"|'[^']*'/g,
            timezone = /\b(?:[PMCEA][SDP]T|(?:Pacific|Mountain|Central|Eastern|Atlantic) (?:Standard|Daylight|Prevailing) Time|(?:GMT|UTC)(?:[-+]\d{4})?)\b/g,
            timezoneClip = /[^-+\dA-Z]/g,
            pad = function (val, len) {
                val = String(val);
                len = len || 2;
                while (val.length < len) val = "0" + val;
                return val;
            };
    
        // Regexes and supporting functions are cached through closure
        return function (date, mask, utc) {
            var dF = dateFormat;
    
            // You can't provide utc if you skip other args (use the "UTC:" mask prefix)
            if (arguments.length == 1 && Object.prototype.toString.call(date) == "[object String]" && !/\d/.test(date)) {
                mask = date;
                date = undefined;
            }
    
            // Passing date through Date applies Date.parse, if necessary
            date = date ? new Date(date) : new Date;
            if (isNaN(date)) throw SyntaxError("invalid date");
    
            mask = String(dF.masks[mask] || mask || dF.masks["default"]);
    
            // Allow setting the utc argument via the mask
            if (mask.slice(0, 4) == "UTC:") {
                mask = mask.slice(4);
                utc = true;
            }
    
            var    _ = utc ? "getUTC" : "get",
                d = date[_ + "Date"](),
                D = date[_ + "Day"](),
                m = date[_ + "Month"](),
                y = date[_ + "FullYear"](),
                H = date[_ + "Hours"](),
                M = date[_ + "Minutes"](),
                s = date[_ + "Seconds"](),
                L = date[_ + "Milliseconds"](),
                o = utc ? 0 : date.getTimezoneOffset(),
                flags = {
                    d:    d,
                    dd:   pad(d),
                    ddd:  dF.i18n.dayNames[D],
                    dddd: dF.i18n.dayNames[D + 7],
                    m:    m + 1,
                    mm:   pad(m + 1),
                    mmm:  dF.i18n.monthNames[m],
                    mmmm: dF.i18n.monthNames[m + 12],
                    yy:   String(y).slice(2),
                    yyyy: y,
                    h:    H % 12 || 12,
                    hh:   pad(H % 12 || 12),
                    H:    H,
                    HH:   pad(H),
                    M:    M,
                    MM:   pad(M),
                    s:    s,
                    ss:   pad(s),
                    l:    pad(L, 3),
                    L:    pad(L > 99 ? Math.round(L / 10) : L),
                    t:    H < 12 ? "a"  : "p",
                    tt:   H < 12 ? "am" : "pm",
                    T:    H < 12 ? "A"  : "P",
                    TT:   H < 12 ? "AM" : "PM",
                    Z:    utc ? "UTC" : (String(date).match(timezone) || [""]).pop().replace(timezoneClip, ""),
                    o:    (o > 0 ? "-" : "+") + pad(Math.floor(Math.abs(o) / 60) * 100 + Math.abs(o) % 60, 4),
                    S:    ["th", "st", "nd", "rd"][d % 10 > 3 ? 0 : (d % 100 - d % 10 != 10) * d % 10]
                };
    
            return mask.replace(token, function ($0) {
                return $0 in flags ? flags[$0] : $0.slice(1, $0.length - 1);
            });
        };
    }();
    
    // Some common format strings
    dateFormat.masks = {
        "default":      "ddd mmm dd yyyy HH:MM:ss",
        shortDate:      "m/d/yy",
        mediumDate:     "mmm d, yyyy",
        longDate:       "mmmm d, yyyy",
        fullDate:       "dddd, mmmm d, yyyy",
        shortTime:      "h:MM TT",
        mediumTime:     "h:MM:ss TT",
        longTime:       "h:MM:ss TT Z",
        isoDate:        "yyyy-mm-dd",
        isoTime:        "HH:MM:ss",
        isoDateTime:    "yyyy-mm-dd'T'HH:MM:ss",
        isoUtcDateTime: "UTC:yyyy-mm-dd'T'HH:MM:ss'Z'"
    };
    
    // Internationalization strings
    dateFormat.i18n = {
        dayNames: [
            "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
            "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
        ],
        monthNames: [
            "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
            "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
        ]
    };
    
    // For convenience...
    Date.prototype.format = function (mask, utc) {
        return dateFormat(this, mask, utc);
    };
//-----------------------------------------------------------------------------


if (!Date.prototype.toISOString) {
  (function() {

    function pad(number) {
      if (number < 10) {
        return '0' + number;
      }
      return number;
    }

    Date.prototype.toISOString = function() {
      return this.getUTCFullYear() +
        '-' + pad(this.getUTCMonth() + 1) +
        '-' + pad(this.getUTCDate()) +
        'T' + pad(this.getUTCHours()) +
        ':' + pad(this.getUTCMinutes()) +
        ':' + pad(this.getUTCSeconds()) +
        '.' + (this.getUTCMilliseconds() / 1000).toFixed(3).slice(2, 5) +
        'Z';
    };

  }());
}
function ISOStr2Date(str)
{
	str=str.replace(/\D/g," ");
	var dt1=str.split(" ");
	//modify month between 1 based ISO and 0 based Date 
	dt1[1]--;
	return new Date(dt1[0],dt1[1],dt1[2],dt1[3],dt1[4],dt1[5]);//Date(Date.UTC(dt1[0],dt1[1],dt1[2],dt1[3],dt1[4],dt1[5]));
}
function str2date(s) // used from dispatch.js as well
{
	//var parts ='04/03/2014'.split('/');
	//var mydate = new Date(parts[2],parts[0]-1,parts[1]); //please put attention to the month
    var s1=s.split(" ");
	var d =s1[0].split('/'); //month/day/year
    var t=s1[1].split(":"); // hour/min
    var h=parseInt(t[0]);
	if(s.indexOf("PM")!=-1)
	   h+=12;
    var date = new Date(d[2],d[0]-1,d[1],h,t[1]);
    return date;
}
//---------------------------------------------------------
function cancelBubble(e) {
 var evt = e ? e:window.event;
 if (evt.stopPropagation)    evt.stopPropagation();
 if (evt.cancelBubble!=null) evt.cancelBubble = true;
}
//------common functions
function g_rand() { return Math.round((Math.random() * 10000)); }
function loadJsCss(fn, ft){//http://www.javascriptkit.com/javatutors/loadjavascriptcss.shtml
 var fref;
 if (ft=="js"){ //if filename is a external JavaScript file
  fref=document.createElement('script')
  fref.setAttribute("type","text/javascript")
  fref.setAttribute("src", fn)
 }
 else if (ft=="css"){ //if filename is an external CSS file
  fref=document.createElement("link")
  fref.setAttribute("rel", "stylesheet")
  fref.setAttribute("type", "text/css")
  fref.setAttribute("href", fn)
 }
 if (typeof fref!="undefined")
  document.getElementsByTagName("head")[0].appendChild(fref)
}

//================================================
function isNDef(a) { return (typeof a == "undefined" || a == null) }
function isEmpty(a) { return (isNDef(a) || a == "") } //isEmpty
function _el(v) { return document.getElementById(v) }
function str2int(s)
{
	return parseInt(s)
}
//check if string s contains substring  case-insensitive
function c_str(s,sb)
{
	var rv=false;
	if (s.toLowerCase().indexOf(sb) != -1)
      rv=true;
	  return rv;
}
function cStr(s,sb)
{
	var s1=sb.split(' ');
	if(s1.length>1)
	{
		
		for (var i in s1) {
		  if(!c_str(s,s1[i]))
		   return false;
		}
		return true;
	}
	else
	  return c_str(s,sb);
}
//===============array=======================
function removeFirstArrEl(arr)
{
	
	var indexToRemove = 0;
	var numberToRemove = 1;
	arr.splice(indexToRemove, numberToRemove);
}

//========================Hex string================================
//http://stackoverflow.com/questions/14603205/how-to-convert-hex-string-into-a-bytes-array-and-a-bytes-array-in-the-hex-strin
function parseHexString(str) {
    var result = [];
    while (str.length >= 2) {
        result.push(parseInt(str.substring(0, 2), 16));

        str = str.substring(2, str.length);   //yourNumber = parseInt(hexString, 16);   hexString = yourNumber.toString(16);
    }

    return result;
}

function createHexString(arr) {
    var result = "";
    var z;

    for (var i = 0; i < arr.length; i++) {
        var str = arr[i].toString(16);

        z = 2 - str.length + 1;
        str = Array(z).join("0") + str;

        result += str;
    }

    return result;
}
function testcreateHexString() {
    alert('test function createHexString');
    var key = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24];  // byte array the value <= 255  !!!!
    var result = createHexString(key);
    alert('Hex value:' + result);

    alert('test function parseHexString');

    var key2 = parseHexString(result);

    if (key.sort().join() == key2.sort().join())
        alert('Matched');

}
//-------------------Byte array conversions-------------------
//In JavaScript bit shifts (>>, <<) are always performed on signed, 32-bits integers. This leads to range overflow for large numbers. !!!
function Arr2Int(buf,index,n)
{
    var val = 0,v;
    // assuming the array has at least four elements
    for (var i = n-1; i >= 0; i--) {
        v = buf[index + i];
        val = val + (v<<8*(n-1-i));
    }
    return val;
}

function Int2Arr(x,n) {   //Int64 - n=8 ,Int32 - n=4
    var arr = [];
    var i = n;
    do {
        arr[--i] = x & (255);
        x = x >> 8;
    } while (i)
    return arr;
}


longToByteArray = function (/*long*/long) {
    // we want to represent the input as a 8-bytes array
    var byteArray = [0, 0, 0, 0, 0, 0, 0, 0];

    for (var index = 0; index < byteArray.length; index++) {
        var byte = long & 0xff;
        byteArray[index] = byte;
        long = (long - byte) / 256;
    }

    return byteArray;
};

byteArrayToLong = function (/*byte[]*/byteArray) {
    var value = 0;
    for (var i = byteArray.length - 1; i >= 0; i--) {
        value = (value * 256) + byteArray[i];
    }

    return value;
};
/*If you need to handle things like bit fields or 32-bit colors, then you need to deal with signed numbers. The javascript function toString(16) will return a negative hex number which is usually not what you want. This function does some crazy addition to make it a positive number.
*/
function NumToHexStr(v)
{  
	if (v < 0)
    {
    	v = 0xFFFFFFFF + v + 1;
    }
  
    return v.toString(16).toUpperCase();
}

function toFar(v)
{
   return (v * ((9.0 / 5.0)) + 32.0);
}