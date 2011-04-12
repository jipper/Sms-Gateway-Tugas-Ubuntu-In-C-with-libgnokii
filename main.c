/* 
 * File:   main.c
 * Author: jipper
 *
 * Created on April 12, 2011, 11:21 PM
 */

#include <stdio.h>
#include <stdlib.h>

 
#include "main.h"
#include "libgnokii_api.h"
 
/*
 * 
 */

struct gn_statemachine *state;
gboolean holdgnokii_init;

gboolean libgtkliststore_outbox_add(pegang *unit)
{
    GtkListStore *handle;
    GtkTreeIter iter;

    handle = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(unit->info_dialog_main->treeview_outbox)));

    gtk_list_store_insert_with_values(handle,&iter,0,
            0,unit->sms_send_encapsulation->no_dest,
            1,unit->sms_send_encapsulation->msg,
            2,unit->sms_send_encapsulation->status,
            -1);

    gtk_tree_view_scroll_to_point(GTK_TREE_VIEW(unit->info_dialog_main->treeview_outbox),0,0);

    return TRUE;

}

gboolean down(pegang *unit)
{

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(unit->info_dialog_main->togglebutton_connect),FALSE);
    
    return FALSE;
}

gboolean init(pegang *unit)
{
    GtkBuilder *main_glade;
    GError *error = NULL;
    GList *list_mode = NULL;

    main_glade = gtk_builder_new();

    if(gtk_builder_add_from_file(main_glade,"main.glade",&error) == FALSE)
    {
        g_printf("error -> %s\n",error->message);
        g_error_free(error);
        return FALSE;
    }

    unit->info_dialog_main->window              = GTK_WIDGET(gtk_builder_get_object(main_glade,"window"));
    unit->info_dialog_main->button_kirim        = GTK_WIDGET(gtk_builder_get_object(main_glade,"button_kirim"));
    unit->info_dialog_main->button_open_setting = GTK_WIDGET(gtk_builder_get_object(main_glade,"button_open_setting"));
    unit->info_dialog_main->entry_nomor_tujuan  = GTK_WIDGET(gtk_builder_get_object(main_glade,"entry_nomor_tujuan"));
    unit->info_dialog_main->entry_pesan_tujuan  = GTK_WIDGET(gtk_builder_get_object(main_glade,"entry_pesan_tujuan"));
    unit->info_dialog_main->progressbar_sinyal  = GTK_WIDGET(gtk_builder_get_object(main_glade,"progressbar_sinyal"));
    unit->info_dialog_main->treeview_outbox     = GTK_WIDGET(gtk_builder_get_object(main_glade,"treeview_outbox"));
 
    unit->info_dialog_main->button_buka_file_settingan  =  GTK_WIDGET(gtk_builder_get_object(main_glade,"button_buka_file_settingan"));
    unit->info_dialog_main->togglebutton_connect        =  GTK_WIDGET(gtk_builder_get_object(main_glade,"togglebutton_connect"));
    gtk_widget_show(unit->info_dialog_main->window);
    gtk_builder_connect_signals(main_glade,unit);
    g_object_unref(G_OBJECT(main_glade));

    return TRUE;
}

int main(int argc, char** argv) {


    gtk_init(&argc,&argv);
    
    pegang *unit                = g_slice_new(pegang);
    unit->info_dialog_main      = g_slice_new(dialog_main);


    
    if(!init(unit))
    {
        return EXIT_FAILURE;
    }

  gtk_main();
    g_slice_free(dialog_main,unit->info_dialog_main);
    g_slice_free(pegang,unit);

  
    return (EXIT_SUCCESS);
}



G_MODULE_EXPORT void keluar(GtkWidget *wi,pegang *unit)
{
    down(unit);
    gtk_main_quit();
}

G_MODULE_EXPORT void on_button_buka_file_settingan_clicked(GtkWidget *widget, pegang *unit)
{
    GtkWidget *dialog;
    gchar *tmp_data = NULL;

    dialog = gtk_file_chooser_dialog_new("Buka Settingan",GTK_WINDOW(unit->info_dialog_main->window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
      
        tmp_data = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        unit->info_dialog_main->file_setting = tmp_data;
        printf("file setting added : %s\n",unit->info_dialog_main->file_setting);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
 
}

G_MODULE_EXPORT void on_togglebutton_connect_toggled(GtkWidget *widget, pegang *unit)
{
    if(unit->info_dialog_main->file_setting)
    {

        if(!unit->info_dialog_main->timeout_connected_phone)
        {
            
            g_printf("firing up...\n");
            unit->info_dialog_main->timeout_connected_phone = g_timeout_add(2000,(GSourceFunc)sms_gateway_sender_api,unit);
        }
         else
        {
            g_source_remove(unit->info_dialog_main->timeout_connected_phone);
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(unit->info_dialog_main->progressbar_sinyal),(float)0.0);
        
            g_printf("down...\n");

            
            unit->info_dialog_main->timeout_connected_phone = FALSE;
        }

        
    }
   
}


G_MODULE_EXPORT void on_button_kirim_clicked(GtkWidget *widget, pegang *unit)
{
    const gchar *destnum;
    const gchar *txt;

    destnum = gtk_entry_get_text(GTK_ENTRY(unit->info_dialog_main->entry_nomor_tujuan));
    txt     = gtk_entry_get_text(GTK_ENTRY(unit->info_dialog_main->entry_pesan_tujuan));

    unit->sms_send_encapsulation = g_slice_new(sms_to_lib);

    gint sms_length = snprintf(unit->sms_send_encapsulation->msg,sizeof(unit->sms_send_encapsulation->msg),"%s",txt);
    unit->sms_send_encapsulation->msg[sms_length] = '\0';

    gint destnum_length = snprintf(unit->sms_send_encapsulation->no_dest,sizeof(unit->sms_send_encapsulation->no_dest),"%s",destnum);
    unit->sms_send_encapsulation->no_dest[destnum_length] = '\0';
    

            if(libgnokii_sms_send_api(unit) == TRUE)
            {
                unit->sms_send_encapsulation->status = "Sent";
                libgtkliststore_outbox_add(unit);
            }
            else
            {
                unit->sms_send_encapsulation->status = "Failed";
                libgtkliststore_outbox_add(unit);
            }
  
    g_slice_free(sms_to_lib,unit->sms_send_encapsulation);

}