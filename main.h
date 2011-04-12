/* 
 * File:   main.h
 * Author: jipper
 *
 * Created on April 12, 2011, 11:23 PM
 */
#include <gnokii.h>
#include <glib-2.0/glib.h>
#include <gtk/gtk.h>
#ifndef MAIN_H
#define	MAIN_H

#ifdef	__cplusplus
extern "C" {
#endif


    typedef struct _dialog_main
    {
        GtkWidget *window;
        GtkWidget *entry_nomor_tujuan;
        GtkWidget *entry_pesan_tujuan;
        GtkWidget *button_kirim;
        GtkWidget *button_open_setting;
        GtkWidget *treeview_outbox;
        GtkWidget *progressbar_sinyal;

        GtkWidget *button_buka_file_settingan;
        GtkWidget *togglebutton_connect;

        gchar *file_setting;

        guint timeout_connected_phone;
        
    }dialog_main;

    typedef struct _sms_to_lib
    {
        char no_dest[50];
        char msg[GN_SMS_MAX_LENGTH];
        char *status;
    } sms_to_lib;


    typedef struct _pegang
    {
        dialog_main *info_dialog_main;

        sms_to_lib *sms_send_encapsulation;
    }pegang;


#ifdef	__cplusplus
}
#endif

#endif	/* MAIN_H */

