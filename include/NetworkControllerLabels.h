#undef AUTOCONNECT_MENULABEL_OPENSSIDS
#define AUTOCONNECT_MENULABEL_OPENSSIDS   "Known Networks"

#undef AUTOCONNECT_PAGECONFIG_SSID
#define AUTOCONNECT_PAGECONFIG_SSID "Network name"

#undef AUTOCONNECT_PAGECONFIG_APPLY
#define AUTOCONNECT_PAGECONFIG_APPLY "Connect"

#undef AUTOCONNECT_PAGECONFIG_ENABLEDHCP
#define AUTOCONNECT_PAGECONFIG_ENABLEDHCP "Automatic configuration"

#undef AUTOCONNECT_MENULABEL_CONFIGNEW
#define AUTOCONNECT_MENULABEL_CONFIGNEW "Setup Network"

#undef AUTOCONNECT_PAGETITLE_STATISTICS
#define AUTOCONNECT_PAGETITLE_STATISTICS "Reader Clock"

#undef AUTOCONNECT_PAGETITLE_CONFIG
#define AUTOCONNECT_PAGETITLE_CONFIG "Setup Network - Reader Clock"

#undef AUTOCONNECT_PAGETITLE_CONNECTING
#define AUTOCONNECT_PAGETITLE_CONNECTING "Connecting... - Reader Clock"

#undef AUTOCONNECT_PAGETITLE_CONNECTIONFAILED
#define AUTOCONNECT_PAGETITLE_CONNECTIONFAILED "Connection failed!- Reader Clock"

#undef AUTOCONNECT_PAGETITLE_CREDENTIALS
#define AUTOCONNECT_PAGETITLE_CREDENTIALS "Known Networks - Reader Clock"

#undef AUTOCONNECT_TEXT_UPDATINGFIRMWARE
#define AUTOCONNECT_TEXT_UPDATINGFIRMWARE "Firmware update \
<style> \
.s_cap { \
	display: block; \
	text-align: left; \
	font-size: small; \
} \
.aux-page input[type='button'], .aux-page button[type='button'] { \
	margin-left: 0px; \
} \
.aux-page label { \
	padding: 0px; \
} \
input[type='button'], button[type='button'] { \
    background-color: #006064; \
    border-color: #006064; \
    width: 15em; \
} \
</style>"

#undef AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION
#define AUTOCONNECT_PAGESTATS_ESTABLISHEDCONNECTION "</td><td></td></tr><tr><td>Connected to network:"

#undef AUTOCONNECT_PAGESTATS_MODE
#define AUTOCONNECT_PAGESTATS_MODE "</td><td></td></tr></tbody></table>\
<div class='pc'><button type='submit' id='setupNetworkButton'>Setup new Network</button></div> \
<script type='text/javascript'> \
(function() { \
    document.getElementById('setupNetworkButton').onclick = function () { \
        location.href = '/_ac/config'; \
    }; \
})(); \
</script> \
<style> \
input[type='submit'], button[type='submit'] { \
	padding: 8px 30px; \
	background-color: #006064; \
	border-color: #006064; \
} \
input[type='button'], input[type='submit'], button[type='submit'], button[type='button'] { \
	font-weight: bold; \
	letter-spacing: 0.8px; \
	color: #fff; \
	border-radius: 2px; \
    cursor: pointer; \
    width: 100% \
} \
.pc { \
    margin: 0 auto; \
    width: 95%; \
} \
table.info { \
    width: 95%; \
    margin: auto; \
} \
</style> \
<!--"

#undef AUTOCONNECT_PAGESTATS_FREEMEM
#define AUTOCONNECT_PAGESTATS_FREEMEM "--><table style='display: none;'><tbody><tr><td>"