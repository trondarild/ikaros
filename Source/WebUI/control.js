var ikc_doc_element = null;
var current_group_path = "";
var timer = null;
var periodic_task = null;
var current_view = 0;
var view_list = [];

// STATE

var realtime = false;
var reconnect_counter = 0;


// COOKIES FOR PERSISTENT STATE

function setCookie(name,value,days)
{
    var date = new Date();
    date.setTime(date.getTime()+(days?days:1)*86400000);
    var expires = "; expires="+date.toGMTString();
	document.cookie = name+"="+value+expires+"; path=/";
}

function getCookie(name)
{
    var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for(var i=0;i < ca.length;i++) {
		var c = ca[i];
		while (c.charAt(0)==' ') c = c.substring(1,c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
	}
	return null;
}

function eraseCookie(name)
{
	createCookie(name,"",-1);
}

function resetCookies()
{
    setCookie('current_view', "0");
    setCookie('root', ""); // or /
    setCookie('inspector',"closed");
}

// UTILITIES

function ignore_data(obj)
{
}



function get(url, callback, timeout, time)
{
    var ajaxRequest = null;
    var ajaxTimeout = null;
    
    ajaxRequest = new XMLHttpRequest();
    ajaxRequest.open("GET", url, true);
    ajaxRequest.setRequestHeader("Cache-Control", "no-cache");
    ajaxRequest.onload = function () { clearTimeout(ajaxTimeout); callback({content:ajaxRequest.responseText}); };
    ajaxTimeout = setTimeout(function () { ajaxRequest.abort(); if(timeout) timeout(); }, 1000);
    ajaxRequest.send();
}



function getXML(url, callback)
{
    var ajaxRequest = null;
    ajaxRequest = new XMLHttpRequest();
    ajaxRequest.open("GET", url, true);
    ajaxRequest.onload = function () { if(callback) callback(ajaxRequest.responseXML); };
    ajaxRequest.send();
}




// CONTROL AND USER INTERFACE

function select_button(i)
{
	var c = document.getElementById("controls");
    for(var j=0; j<c.children.length; j++)
        c.children.item(j).setAttribute("class","button");
    if(c.children.item(i))
        c.children.item(i).setAttribute("class","selected");
}



function handle_reconnection()
{
    window.location.href=window.location.href;
}



function poll_reconnect()
{
	clearTimeout(periodic_task);

    reconnect_counter++;
    if(reconnect_counter % 2 == 0)
        document.getElementById("iteration").textContent = "◐"; //"◐";
    else
        document.getElementById("iteration").textContent = "◑"; //"◑";

    get("update", handle_reconnection, poll_reconnect, 500);
}



function do_stop()
{
	function handle_data_object(obj)
	{
        select_button(-1);
        document.getElementById("iteration").textContent = ".";
        poll_reconnect();
	}

	clearTimeout(periodic_task);	
	running = false;
    realtime = false;
    
	get("stop", handle_data_object);
}



function do_pause()
{
	function handle_data_object(obj)
	{
        try
        {
            var data = eval("("+obj.content+")");
            var v = document.getElementById("view").contentWindow;
            if(data && v && v.update) v.update(data);
            document.getElementById("iteration").textContent = data.iteration;
        }
        catch(err)
        {
            //alert("Error: "+err.message);
        }
	}

	clearTimeout(periodic_task);	
    select_button(1);
	running = false;
    realtime = false;
    
	get("pause", handle_data_object);
}



function step()
{
	function handle_data_object(obj)
	{
        try
        {
            var data = eval("("+obj.content+")");
            var v = document.getElementById("view").contentWindow;
            if(data && v && v.update) v.update(data);
            document.getElementById("iteration").textContent = data.iteration;
        }
        catch(err)
        {
            //alert("Error: "+err.message);
        }
	}

    get("step", handle_data_object);
}



function do_step()
{
	clearTimeout(periodic_task);
    select_button(2);
	running = false;
    realtime = false;
	step();
}



function runstep()
{
    if(!running)
        return;

    function handle_data_object(obj)
    {
        try
        {
            var data = eval("("+obj.content+")");
            var v = document.getElementById("view").contentWindow;
            if(data && v && v.update)
                v.update(data);
            document.getElementById("iteration").textContent = data.iteration;
            if(running)
                setTimeout("runstep();", 1);
        }
        catch(err)
        {
            // Connection broken!!!
            poll_reconnect();
        }
    }

    get("runstep", handle_data_object, poll_reconnect, 1000);
}



function do_run()
{
	clearTimeout(periodic_task);
    select_button(3);
	running = true;
    realtime = false;
    runstep();
}



function update()
{
	function handle_data_object(obj)
	{
        try
        {
            var data = eval("("+obj.content+")");
            var v = document.getElementById("view").contentWindow;
            if(data && v && v.update) v.update(data);
            document.getElementById("iteration").textContent = data.iteration;
        
            if(data.state == 3)
                do_run();
            else if(data.state == 4)
                setTimeout("update();", 100);
        }
        catch(err)
        {
            poll_reconnect();
        }
	}

	get("update", handle_data_object, poll_reconnect, 1000);
}



function do_update()
{
	var b = document.getElementsByTagName("image");
	update();
}



function do_realtime()
{
	clearTimeout(periodic_task);
    select_button(4);
	running = false;
    realtime = true;
	get("realtime", ignore_data);
    setTimeout("update();", 10);
}



function getChildrenByTagName(element, name)
{
    if(!element)
        return null;
    
    if(!element.childNodes)
        return null;
    
    var list = new Array();
    for(var i=0; i<element.childNodes.length; i++)
        if (element.childNodes[i].nodeName==name)
            list.push(element.childNodes[i]);
            
    return list;
}



function getGroupWithName(element, name)
{
    if(!element)
        return null;
    
    if(!element.childNodes)
        return null;

    for(var i=0; i<element.childNodes.length; i++)
        if (element.childNodes[i].nodeName=="group" &&  element.childNodes[i].getAttribute("name")==name)
            return element.childNodes[i];

    return null;
}



function build_group_list_OLD(group, list, p, top, selected_element)
{
    if(!group)
        return;

    if(!list)
        return;

    for(i in group)
    {
        var name = group[i].getAttribute("title");
        if(!name)
            name = group[i].getAttribute("name");
        if(!name)
            name = "Untitled";

        var ip= (top ? "" : p + "/" + name);

        var subgroups = getChildrenByTagName(group[i], "group");

        var li = document.createElement("li");
        var bar  = document.createElement("span"); 
        var tri_span = document.createElement("span");
        var span = document.createElement("span");

        var triangle;
        tri_span.setAttribute("class","group-closed");
        tri_span.path = (top ? "" : ip);

        bar.appendChild(tri_span);

        var txt = document.createTextNode(name);
        span.appendChild(txt);
        span.path = (top ? "" : ip);
        
        if(top && !selected_element)
        {
            bar.setAttribute("class","group-bar-selected");
            span.setAttribute("class","group-selected");
            tri_span.setAttribute("class","group-closed");
        }
        else if(top)
        {
            bar.setAttribute("class","group-bar");
            span.setAttribute("class","group-unselected");
            tri_span.setAttribute("class","group-open");
        }
        else if(selected_element && name == selected_element[0])
        {
            bar.setAttribute("class","group-bar-selected");
            span.setAttribute("class","group-selected");
        }
        else
        {
            bar.setAttribute("class","group-bar");
            span.setAttribute("class","group-unselected");
        }

        bar.appendChild(span);

        li.appendChild(bar);

        if(subgroups.length>0)
        {
            if(tri_span.getAttribute("class")=="group-closed")
            {
                triangle = document.createTextNode("▷ ");
                tri_span.appendChild(triangle);
            }
            else
            {
                triangle = document.createTextNode("▽ ");
                tri_span.appendChild(triangle);
            }
            
            var ul = document.createElement("ul");
            
            if(tri_span.getAttribute("class") == "group-closed")
                ul.setAttribute("class", "hidden");
            else
                ul.setAttribute("class", "visible");
            
            if(selected_element) selected_element.shift();
            build_group_list(subgroups, ul, ip, null, selected_element);
            li.appendChild(ul);
        }

        list.appendChild(li);
    }
}



function build_group_list(group, list, p, top, selected_element, depth)
{
    if(!list)
        return;

    if(!group)
        return;

    var current = selected_element.shift();
    var selected = (selected_element.length == 0 ? current : "");
    
    for(i in group)
    {
        name = group[i].getAttribute("name");
        if(!name)
            name = "Untitled";

        var ip = (top ? "" : p + "/" + name);
        var subgroups = getChildrenByTagName(group[i], "group");

        // Create list elements
        
        var li = document.createElement("li");
        var bar  = document.createElement("span"); 
        var tri_span = document.createElement("span");
        var span = document.createElement("span");
        var txt = document.createTextNode(name);

        // Connect top-down
        
        list.appendChild(li);
        li.appendChild(bar);
        bar.appendChild(tri_span);
        bar.appendChild(span);
        span.appendChild(txt);

        // Set attributes

        var is_selected = (top && selected_element.length==0) || (name == selected);
        bar.setAttribute("class", (is_selected ? "group-bar-selected" : "group-bar"));
        span.setAttribute("class", (is_selected ? "group-selected" : "group-unselected"));
        tri_span.setAttribute("class", ((top && selected_element.length>0) || (name == current && !is_selected) ? "group-open" : "group-closed"));
        
        tri_span.path = (top ? "" : ip);
        span.path = (top ? "" : ip);
        
        // Create ul for subgroups

        if(subgroups.length > 0)
        {
            var ul = document.createElement("ul");
            li.appendChild(ul);

            if(tri_span.getAttribute("class")=="group-closed")
            {
                var triangle = document.createTextNode("▷ ");
                tri_span.appendChild(triangle);
                ul.setAttribute("class", "hidden");
            }
            else
            {
                var triangle = document.createTextNode("▽ ");
                tri_span.appendChild(triangle);
                ul.setAttribute("class", "visible");
            }
            
            build_group_list(subgroups, ul, ip, null, selected_element, depth+1);
        }
    }
}



function build_view_list_with_editor(view)
{
    if(!view)
        return;
    
    view_list = view;

    var vc = document.getElementById("viewdots");
 
    if(!vc)
        return;
    
    while(vc.firstChild)
        vc.removeChild(vc.firstChild);

    for(i in view_list)
    {
        var a = document.createElement("a");
        var dot = document.createTextNode("○ ");
        a.appendChild(dot);
        vc.appendChild(a);
        
        a.setAttribute("onclick","change_view("+i+")");
        a.setAttribute("title",view_list[i].getAttribute("name"));
    }
    
    // add edit view
    
    var a = document.createElement("a");
    var dot = document.createTextNode("◽ ");
    a.appendChild(dot);
    vc.appendChild(a);
    a.setAttribute("onclick","change_view("+view_list.length+")");
    a.setAttribute("title","Inspector");

    // add inspector view
    
    var a = document.createElement("a");
    var dot = document.createTextNode("ℹ ");
    a.appendChild(dot);
    a.style.lineHeight="80%";
    vc.appendChild(a);
        
    a.setAttribute("onclick","change_view("+(view_list.length+1)+")");
    a.setAttribute("title","Editor");
 }


function build_view_list(view)
{
    if(!view)
        return;
    
    view_list = view;

    var vc = document.getElementById("viewdots");
 
    if(!vc)
        return;
    
    while(vc.firstChild)
        vc.removeChild(vc.firstChild);

    for(i in view_list)
    {
        var a = document.createElement("a");
        var dot = document.createTextNode("○ ");
        a.appendChild(dot);
        vc.appendChild(a);
        
        a.setAttribute("onclick","change_view("+i+")");
        a.setAttribute("title",view_list[i].getAttribute("name"));
    }
    
    // add inspector view
    
    var a = document.createElement("a");
    var dot = document.createTextNode("ℹ ");
    a.appendChild(dot);
    a.style.lineHeight="80%";
    vc.appendChild(a);
        
    a.setAttribute("onclick","change_view("+(view_list.length)+")");
    a.setAttribute("title","Editor");
}



function change_view_with_editor(index)
{
    current_view = index;

    var vc = document.getElementById("viewdots");
    var alist = getChildrenByTagName(vc, "A");
    for(i in alist)
        if(i == alist.length-2)
            alist[i].innerHTML = (i == index ? "ℹ " : "ℹ ");
        else if(i==alist.length-1)
            alist[i].innerHTML = (i == index ? "◾ " : "◽ ");
        else
            alist[i].innerHTML = (i == index ? "● " : "○ ");
        
    var v = document.getElementById("view");
    if(!v) return;
    
    if(index==alist.length-2)
        v.setAttribute("src", "http://"+location.host+current_group_path+"/inspector.html");
    else if(index==alist.length-1)
        v.setAttribute("src", "http://"+location.host+current_group_path+"/editor.svg");
    else
        v.setAttribute("src", "http://"+location.host+"/view"+current_group_path+"/view"+current_view+".svg");
    
    var vn = document.getElementById("viewname");
    if(vn)
    {        
        if(index==alist.length-2)
            vn.innerHTML = "Inspector"
        else if(index==alist.length-1)
            vn.innerHTML = "Editor"
        else
        {
            var vw = view_list[current_view];
            if(vw)
                vn.innerHTML = vw.getAttribute("name");
            else
                vn.innerHTML = "View "+index;
        }
    }
}



function change_view(index)
{
    current_view = index;
    setCookie('current_view', current_view);

    var vc = document.getElementById("viewdots");
    var alist = getChildrenByTagName(vc, "A");
    for(i in alist)
        if(i == alist.length-1)
            alist[i].innerHTML = (i == index ? "ℹ " : "ℹ ");
        else
            alist[i].innerHTML = (i == index ? "● " : "○ ");
        
    var v = document.getElementById("view");
    if(!v) return;
    
    if(index==alist.length-1)
        v.setAttribute("src", "http://"+location.host+current_group_path+"/inspector.html");
    else
        v.setAttribute("src", "http://"+location.host+"/view"+current_group_path+"/view"+current_view+".html");
    
    var vn = document.getElementById("viewname");
    if(vn)
    {        
        if(index==alist.length-1)
            vn.innerHTML = "Inspector"
        else
        {
            var vw = view_list[current_view];
            if(vw)
            {
                var n = vw.getAttribute("name");
                if(n)
                    vn.innerHTML = n;
                else
                    vn.innerHTML = "View "+index
            }
            else
                vn.innerHTML = "View "+index;
        }
    }
}



function restore_view()
{
    var v = getCookie('current_view');
    change_view(v?parseInt(v):0);
}



function next_view()
{
    if(current_view < view_list.length+1)
        change_view(current_view+1);
}



function prev_view()
{
    if(current_view > 0)
        change_view(current_view-1);
}



function update_group_list_and_views()
{
	function handle_data_object(xml)
	{
        try
        {
            ikc_doc_element = xml.documentElement;
            var title = document.getElementById("title");
            
            var name = ikc_doc_element.getAttribute("title");
            if(!name)
                name = ikc_doc_element.getAttribute("name");
            if(!name)
                name = "Untitled";

            if(title) title.textContent = name;
            
            grouplist = document.getElementById("grouplist");
            current_group_path = getCookie('root');
            if(!current_group_path)
                current_group_path ="";
            get("/setroot"+current_group_path, ignore_data);
            build_group_list([xml.documentElement], grouplist, "", true, (current_group_path?current_group_path.split('/'):[]), 0);
            
            var p = current_group_path.split('/');
            p.shift();
            var e = ikc_doc_element;
            for(i in p)
                e = getGroupWithName(e, p[i]);
            build_view_list(getChildrenByTagName(e, "view"));
            
            restore_view();
        }
        catch(err)
        {
            alert("Error in update_group_list_and_views. XML could not be parsed (possibly because of an unknown entitiy): "+err.message);
        }
	}

	getXML("xml.ikc", handle_data_object);
}



function toggle_inspector()
{
	if(document.getElementById('pane').style.width == "0px")
    {
        document.getElementById('pane').style.width='300px';
        document.getElementById('split').src="/Icons/single.png";
        setCookie('inspector','open');
	}
    else
	{
        document.getElementById('pane').style.width='0px';
        document.getElementById('split').src="/Icons/split.png";
        setCookie('inspector','closed');
    }
}



function restore_inspector()
{
    if(getCookie('inspector') == 'open')
    {
        document.getElementById('pane').style.width='300px';
        document.getElementById('split').src="/Icons/single.png";
	}
    else
	{
        document.getElementById('pane').style.width='0px';
        document.getElementById('split').src="/Icons/split.png";
    }
}



function toggle(e)
{
	if(e.target.getAttribute("class") == "group-open")
    {
		e.target.setAttribute('class', 'group-closed');
        e.target.textContent = "▷ ";

        var ul = e.target.parentElement.parentElement.children.item(1);
        if(ul)
            ul.setAttribute('class', 'hidden');
    }
    
	else if(e.target.getAttribute("class") == "group-closed")
    {
		e.target.setAttribute('class', 'group-open');
        e.target.textContent = "▽ ";

        var ul = e.target.parentElement.parentElement.children.item(1);
        if(ul)
            ul.setAttribute('class', 'visible');
    }
    
    else if(e.target.getAttribute("class") == "group-unselected")
    {
        var s = document.getElementsByClassName("group-selected");
        while(s.length > 0)
        {
            var si = s[0];
            si.setAttribute("class", "group-unselected");
            si.parentElement.setAttribute("class", "group-bar");
        }
        
		e.target.setAttribute("class", "group-selected");
		e.target.parentElement.setAttribute("class", "group-bar-selected");

        current_group_path = e.target.path;
        get("/setroot"+current_group_path, ignore_data);
        
        setCookie('root', current_group_path);

        var p = e.target.path.split('/');
        p.shift();
        var e = ikc_doc_element;
        for(i in p)
            e = getGroupWithName(e, p[i]);   
        build_view_list(getChildrenByTagName(e, "view"));
        change_view(0);
    }

	if (e.stopPropagation) e.stopPropagation();
}



function restore_root()
{
    alert("Root: "+getCookie('root'));
}



// Call after load

if(!document.cookie)
    resetCookies();

var grouplist = document.getElementById("grouplist");
grouplist.addEventListener('click', toggle, false);

update_group_list_and_views();

// Restor state after reload

restore_inspector();
//restore_view(); // not really needed is it?
//restore_root();

update();

// AUTOSTART
// do_run();
