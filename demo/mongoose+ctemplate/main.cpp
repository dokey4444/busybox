/*
// =====================================================================================
// 
//       Filename:  main.cpp
// 
//    Description:  
// 
//        Version:  1.0
//        Created:  11/21/2016 05:03:21 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
//        Company:  
// 
// =====================================================================================
*/


#include <cstdlib>
#include <iostream>

#include <string>
#include <vector>

#include "mongoose.h"
#include "ctemplate/template.h"

static const char *s_http_port = "8080";
static struct mg_serve_http_opts s_http_server_opts;

static void handle_ajax_sum(struct mg_connection *nc, struct http_message *hm) {
	char n1[100], n2[100];
	double result;

	/*  Get form variables */
	mg_get_http_var(&hm->body, "n1", n1, sizeof(n1));
	mg_get_http_var(&hm->body, "n2", n2, sizeof(n2));

	/*  Send headers */
	mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

	/*  Compute the result and send it back as a JSON object */
	result = strtod(n1, NULL) + strtod(n2, NULL);
	mg_printf_http_chunk(nc, "{ \"result\": %lf }", result);
	mg_send_http_chunk(nc, "", 0); /*  Send empty chunk, the end of response */
}

/* 
// ===  FUNCTION  ======================================================================
//         Name:  handle_template
//  Description:   
// =====================================================================================
*/
static void handle_template(struct mg_connection *nc, struct http_message *hm)
{
	ctemplate::TemplateDictionary dict("example");
	dict.SetFormattedValue("Title", "This is the %s", "template page");

	// For now, assume everyone lives in CA.
	// (Try running the program with a 0 here instead!)
  	if (1) {
  		dict.ShowSection("IN_CA");
  	}
	ctemplate::TemplateDictionary *dict1 = dict.AddSectionDictionary( "IN_CA" );
  	dict1->SetValue("Name", "A");
  	dict1->SetIntValue("Age", 1);
	ctemplate::TemplateDictionary *dict2 = dict.AddSectionDictionary( "IN_CA" );
  	dict2->SetValue("Name", "B");
  	dict2->SetIntValue("Age", 2);
	ctemplate::TemplateDictionary *dict3 = dict.AddSectionDictionary( "IN_CA" );
  	dict3->SetValue("Name", "C");
  	dict3->SetIntValue("Age", 3);

	std::string output;
	ctemplate::ExpandTemplate("example.tpl", ctemplate::DO_NOT_STRIP, &dict, &output);
	std::cout << output;

	/*  Send headers */
	mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");
	mg_printf_http_chunk(nc, "%s", output.c_str());
	mg_send_http_chunk(nc, "", 0); /*  Send empty chunk, the end of response */
}		// -----  end of static function handle_template  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  handle_template
//  Description:   
// =====================================================================================
*/
static void handle_ajax(struct mg_connection *nc, struct http_message *hm)
{
	mg_http_serve_file(nc, hm, "./ajax.html", mg_mk_str("text/html"), mg_mk_str(""));
}		// -----  end of static function handle_ajax  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  handle_raw
//  Description:   
// =====================================================================================
*/
static void handle_raw(struct mg_connection *nc, struct http_message *hm)
{
	char x[BUFSIZ], y[BUFSIZ];
	double result;

	/*  Get form variables */
	mg_get_http_var(&hm->query_string, "x", x, sizeof(x));
	mg_get_http_var(&hm->query_string, "y", y, sizeof(y));

	/*  Send headers */
	mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n\r\n");

	/*  Compute the result and send it back as a JSON object */
	result = strtod(x, NULL) + strtod(y, NULL);
	mg_printf_http_chunk(nc, "<html>");
	mg_printf_http_chunk(nc, "<head>");
	mg_printf_http_chunk(nc, "<title>Raw Page</title>");
	mg_printf_http_chunk(nc, "</head>");
	mg_printf_http_chunk(nc, "<body>");
	mg_printf_http_chunk(nc, "<h1>Result is %lf</h1>", result);
	mg_printf_http_chunk(nc, "</body>");
	mg_printf_http_chunk(nc, "</html>");
	mg_send_http_chunk(nc, "", 0); /*  Send empty chunk, the end of response */
}		// -----  end of static function handle_raw  -----

/* 
// ===  FUNCTION  ======================================================================
//         Name:  ev_handler
//  Description:   
// =====================================================================================
*/
static void ev_handler(struct mg_connection *nc, int ev, void *ev_data)
{
	struct http_message *hm = (struct http_message *) ev_data;

	switch (ev) {
	case MG_EV_HTTP_REQUEST:

		std::cout << hm->message.p << std::endl;

		if (mg_vcmp(&hm->uri, "/template") == 0) {
			std::cout << "template" << std::endl;
			handle_template(nc, hm);
		} else if (mg_vcmp(&hm->uri, "/raw") == 0) {
			std::cout << "raw" << std::endl;
			handle_raw(nc, hm);
		} else if (mg_vcmp(&hm->uri, "/ajax") == 0) {
			std::cout << "ajax" << std::endl;
			handle_ajax(nc, hm);
		} else if (mg_vcmp(&hm->uri, "/ajax/sum") == 0) {
			std::cout << "ajax sum" << std::endl;
			handle_ajax_sum(nc, hm);
		} else {
			std::cout << "static" << std::endl;
			mg_serve_http(nc, hm, s_http_server_opts); /*  Serve static content */
		}
		break;
	default:
		break;
	}
}

/* 
// ===  FUNCTION  ======================================================================
//         Name:  main
//  Description:  
// =====================================================================================
*/
int main(int argc, char *argv[])
{
	struct mg_mgr mgr;
	struct mg_connection *nc;
	struct mg_bind_opts bind_opts;
	int i;
	char *cp;
	const char *err_str;
#if MG_ENABLE_SSL
	const char *ssl_cert = NULL;
#endif

	mg_mgr_init(&mgr, NULL);

	/*  Use current binary directory as document root */
	if (argc > 0 && ((cp = strrchr(argv[0], DIRSEP)) != NULL)) {
		*cp = '\0';
		s_http_server_opts.document_root = argv[0];
	}

	/*  Process command line options to customize HTTP server */
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-D") == 0 && i + 1 < argc) {
			mgr.hexdump_file = argv[++i];
		} else if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
			s_http_server_opts.document_root = argv[++i];
		} else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
			s_http_port = argv[++i];
		} else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
			s_http_server_opts.auth_domain = argv[++i];
#if MG_ENABLE_JAVASCRIPT
		} else if (strcmp(argv[i], "-j") == 0 && i + 1 < argc) {
			const char *init_file = argv[++i];
			mg_enable_javascript(&mgr, v7_create(), init_file);
#endif
		} else if (strcmp(argv[i], "-P") == 0 && i + 1 < argc) {
			s_http_server_opts.global_auth_file = argv[++i];
		} else if (strcmp(argv[i], "-A") == 0 && i + 1 < argc) {
			s_http_server_opts.per_directory_auth_file = argv[++i];
		} else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
			s_http_server_opts.url_rewrites = argv[++i];
#if MG_ENABLE_HTTP_CGI
		} else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
			s_http_server_opts.cgi_interpreter = argv[++i];
#endif
#if MG_ENABLE_SSL
		} else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
			ssl_cert = argv[++i];
#endif
		} else {
			fprintf(stderr, "Unknown option: [%s]\n", argv[i]);
			exit(1);
		}
	}

	/*  Set HTTP server options */
	memset(&bind_opts, 0, sizeof(bind_opts));
	bind_opts.error_string = &err_str;
#if MG_ENABLE_SSL
	if (ssl_cert != NULL) {
		bind_opts.ssl_cert = ssl_cert;
	}
#endif
	nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
	if (nc == NULL) {
		fprintf(stderr, "Error starting server on port %s: %s\n", s_http_port,
				*bind_opts.error_string);
		exit(1);
	}

	mg_set_protocol_http_websocket(nc);
	s_http_server_opts.enable_directory_listing = "yes";

	printf("Starting RESTful server on port %s, serving %s\n", s_http_port,
			s_http_server_opts.document_root);
	for ( ; ; ) {
		mg_mgr_poll(&mgr, 1000);
	}
	mg_mgr_free(&mgr);

	return EXIT_SUCCESS;
}				// ----------  end of function main  ----------

