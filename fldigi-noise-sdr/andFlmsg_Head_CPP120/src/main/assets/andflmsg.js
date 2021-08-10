//JavaScript for all webview web pages 
//Links back to "JavascriptAccess" class in AndFlmsg.java


//Scan through all elements in the FIRST Form element of the document
function getInputFields() {
	var x = document.forms[0];
	var text = "";
	var i;
	var mychecked = "";
	for (i = 0; i < x.length ;i++) {
		//Skip buttons in the form
		//if ((x.elements[i].getAttribute('type') != 'hidden') && (x.elements[i].type != 'button')) {
		if ((x.elements[i].type != 'button')) {
			//Process each input type differently
			if (x.elements[i].type == 'radio') {
				if (x.elements[i].checked) {
					mychecked = "CHECKED";
				} else {
					mychecked = "";
				}
				if (mychecked.length > 0) {
					text += x.elements[i].name + "." + x.elements[i].value + "," + mychecked + "\n";
				}
			} else if (x.elements[i].type == 'checkbox') {
				if (x.elements[i].checked) {
					mychecked = "ON";
				} else {
					mychecked = "";
				}
				if (mychecked.length > 0) {
					text += x.elements[i].name + "," + mychecked + "\n";
				}
			} else if (x.elements[i].type == 'textarea') {
				if (x.elements[i].value.length > 0) {
					//In Flmsg, text is escaped, then appended (not enclosed in quotes).
					//text += x.elements[i].name + "," + "\x22" + x.elements[i].value + "\x22" + "\n";
					var escapedField = "" + window.JavascriptAccess.escape(x.elements[i].value);
					text += x.elements[i].name + "," + escapedField + "\n";
				}
			} else {
				if (x.elements[i].value.length > 0) {
					var escapedField = "" + window.JavascriptAccess.escape(x.elements[i].value);
					text += x.elements[i].name + "," + escapedField + "\n";
				}
			}
		}
	}
	//Now add any attached "src" attribute for the (self defined) "image" class
	//This returns the full path and file name so that it can be extracted and base64 encoded
	//x = document.getElementsByClassName("image");
	//for (i = 0; i < x.length ;i++) {
	//	if (x[i].src.length > 0) {
	//		text += x[i].name + "," + x[i].src + "\n";
	//	}
	//}
	return text;	
}



//A better visual appearance for making forms read-only than by program (not 
//	used at present as it can't be sent over email)
function setAllReadOnly() { 
	var x = document.forms[0];
    var i; 
    for (i = 0; i < x.length ;i++) { 
        if (x.elements[i].type == 'radio') { 
            if (!x.elements[i].checked) {           
                 x.elements[i].disabled = true; 
            } 
        } else if (x.elements[i].type == 'checkbox') { 
            if (!x.elements[i].checked) {           
                 x.elements[i].disabled = true; 
            } else { 
                 x.elements[i].onclick=function(){return false;}; 
            } 
        } else if (x.elements[i].type == 'select-one') { 
            x.elements[i].disabled = true; 
        } else { 
	        x.elements[i].readOnly = true; 
        } 
    } 
}



//============== From JavaScript to Java ==================


function queueForTx(newDoc) {
	//Selects the first form in the document, then scans all input elements to
	//  returns the pairs (name or name + value) plus (value or Checked/On) flag in a string 
	//
	var action = "Queue";
	var text = getInputFields();
	window.JavascriptAccess.submitFormData(text, action, newDoc);
}



function saveToOutbox(newDoc) {
	//Selects the first form in the document, then scans all input elements to
	//  returns the pairs (name or name + value) plus (value or Checked/On) flag in a string 
	//
	var action = "Outbox";
	var text = getInputFields();
	window.JavascriptAccess.submitFormData(text, action, newDoc);
}



function saveToDrafts(newDoc) {
	//Selects the first form in the document, then scans all input elements to
	//  returns the pairs (name or name + value) plus (value or Checked/On) flag in a string 
	//
	var action = "Drafts";
	var text = getInputFields();
	window.JavascriptAccess.submitFormData(text, action, newDoc);
}


function saveAsTemplate(newDoc) {
	//Selects the first form in the document, then scans all input elements to
	//  returns the pairs (name or name + value) plus (value or Checked/On) flag in a string 
	//
	var action = "Templates";
	var text = getInputFields();
	window.JavascriptAccess.submitFormData(text, action, newDoc);
}



function discardInput() {
	var text = "";
	var action = "Discard";
	var resp = confirm("Do you really want to\n  discard your data?");
	if (resp == true) {
		window.JavascriptAccess.submitFormData(text, action, "");
	}
}



//Prefill the specified input element with the current Date
function dateprefill(fieldname) { 
	var fieldtoprefill = document.getElementsByName(fieldname)[0];
	var mydate = window.JavascriptAccess.dateprefill();
	fieldtoprefill.value = mydate;
}


//Prefill the specified input element with the current Time
function timeprefill(fieldname) { 
	var fieldtoprefill = document.getElementsByName(fieldname)[0];
	var mytime = window.JavascriptAccess.timeprefill();
	fieldtoprefill.value = mytime;
}


//Prefill the specified input element with the current date / Time combination
function datetimeprefill(fieldname) { 
	var fieldtoprefill = document.getElementsByName(fieldname)[0];
	var mydatetime = window.JavascriptAccess.datetimeprefill();
	fieldtoprefill.value = mydatetime;
}


//Call the Popup dialog for ARL formatted pre-defined messages
function arlmessagesdialog(fieldname) { 
	window.JavascriptAccess.arlmessagesdialog(fieldname);
}


//Call the Popup dialog for ARL HX (Delivery) messages
function arlhxdialog(fieldname) { 
	window.JavascriptAccess.arlhxcodesdialog(fieldname);
}


//Call the Popup dialog for updating the "Check" field
function arlmsgcheck(msgfieldname, checkfieldname, stdftmfieldname) {
	var msgfieldcontent = document.getElementsByName(msgfieldname)[0].value;
	if (stdftmfieldname == "") {
		var stdformat = "ON"; //If no field to check, assumes true (to uppercase and limit words per line)
	} else {
		var stdformat = "" + document.getElementsByName(stdftmfieldname)[0].value;
	}
	window.JavascriptAccess.arlmsgcheck(msgfieldname, msgfieldcontent, checkfieldname, stdformat);
}


function editinspreadsheet() {
	//Selects the first form in the document, then scans all input elements to
	//  returns the pairs (name or name + value) plus (value or Checked/On) flag in a string 
	//
	var text = getInputFields();
	window.JavascriptAccess.editInSpreadsheet(text);
}



//Call a Java popup to load an image in the HTML form
//fieldname is the <img> field to be updated
function loadimage(fieldname) { 
	var imgfield = document.getElementsByName(fieldname)[0];
	var fullimagefilename = window.JavascriptAccess.pickImage(fieldname);
	// Setting is now done in dialog listener
	// imgfield.src = "file://" + fullimagefilename;
}



//Call a Java popup to get the GPS position from the last attached picture
//fieldname is the <img> field to be used for extracting data
function getposfromimage(imageFieldName, latFieldName, lonFieldName) { 
	var latFieldElement = document.getElementsByName(latFieldName)[0];
	var lonFieldElement = document.getElementsByName(lonFieldName)[0];
	var imageFieldValue = "";
	//var x = document.getElementsByClassName('image');
	var x = document.getElementsByTagName("img");
	var i;
	for (i = 0; i < x.length; i++) {
		if (x[i].name == imageFieldName) {
			imageFieldValue = "" + x[i].src;
		}
	}
	var latLonString = window.JavascriptAccess.getPosFromImage(imageFieldValue);
	//Split the string and assign values
	var latlon = latLonString.split("|");
	latFieldElement.value = latlon[0];
	lonFieldElement.value = latlon[1];
}



//============== From Java to JavaScript ==================


//Called from Java: append a string to an existing field
function appendToField(targetField, fieldData) { 
	var fieldtoprefill = document.getElementsByName(targetField)[0];
	fieldtoprefill.value = fieldtoprefill.value + " " + fieldData;
}


//Called from Java: Set a field with the value of a string
//Used for formatting special fields in forms
function setField(targetField, fieldData) { 
	var fieldtoprefill = document.getElementsByName(targetField)[0];
	fieldtoprefill.value = "" + fieldData;
}


//Called from Java: Set a field with the source (src) of a string
//Used for setting (and displaying) the selected image in a form
function setImgSource(targetField, fieldData) { 
	//Better way of finding the 'IMG' elements
	//var x = document.getElementsByClassName('image');
	var x = document.getElementsByTagName("img");
	var i;
	for (i = 0; i < x.length; i++) {
		if (x[i].name == targetField) {
			x[i].src = "" + fieldData;
		}
	}
}


//Called from Java: Set a field with value of a string
//Used for setting the selected Textarea containing the base64 encoded image
function setImgTextarea(targetField, fieldData) { 
	//var x = document.getElementsByName(targetField);
	//var i;
	//for (i = 0; i < x.length; i++) {
	//	if(x[i].type == 'textarea') {
	//		x[i].value = "" + fieldData;
	//	}
	//}
	var y = document.forms[0];
	var j = y.length;
	while (j--) {
		if (y.elements[j].name == targetField) {
			y.elements[j].value = fieldData;
		}
	}
}

