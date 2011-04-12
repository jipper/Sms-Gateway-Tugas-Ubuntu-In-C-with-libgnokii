/* 
 * File:   libgnokii_api.c
 * Author: jipper
 *
 * Created on April 12, 2011, 11:51 PM
 */

#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "libgnokii_api.h"

 
/*
 * 
 */
extern gboolean holdgnokii_init;
extern struct gn_statemachine *state;


gn_gsm_number_type libgnokii_nomorhp_tipe_api(const gchar *number)
{

	gn_gsm_number_type type;

	if (!number)
		return GN_GSM_NUMBER_Unknown;
	if (*number == '+') {
		type = GN_GSM_NUMBER_International;
		number++;
	} else {
		type = GN_GSM_NUMBER_Unknown;
	}
	while (*number) {
		if (!isdigit(*number))
			return GN_GSM_NUMBER_Alphanumeric;
		number++;
	}
	return type;

}
gn_error libgnokii_bacatext_api(gn_sms_user_data *udata,pegang *unit)
{
    char message_buffer[255 * GN_SMS_MAX_LENGTH];
    int chars_read;

#ifndef WIN32
    if(isatty(0))
#endif
        sprintf(message_buffer,"%s",unit->sms_send_encapsulation->msg);
        udata->length = snprintf(udata->u.text , sizeof(udata->u.text),"%s",message_buffer);

    return GN_ERR_NONE;
}

gboolean libgnokii_sms_send_api(pegang *unit)
{
    if(!holdgnokii_init)
    {
        printf("hold_gnokii_init aint initialized\n");
        return FALSE;
    }
    //avoiding
    if(unit->sms_send_encapsulation->no_dest == NULL || strlen(unit->sms_send_encapsulation->no_dest) < 2 || unit->sms_send_encapsulation->msg == NULL || strlen(unit->sms_send_encapsulation->msg) < 2)
    {
        return FALSE;
    }

    gn_sms sms;
    gn_error error;
    gn_data data;
    int pos = 0;

    //memset to zero
    gn_sms_default_submit(&sms);
     gn_data_clear(&data);


    //set the dest number
    snprintf(sms.remote.number,sizeof(sms.remote.number),"%s",unit->sms_send_encapsulation->no_dest);
    sms.remote.type = libgnokii_nomorhp_tipe_api(unit->sms_send_encapsulation->no_dest);
    if(sms.remote.type == GN_GSM_NUMBER_Alphanumeric)
    {
        printf("Invalid Phone Number\n");
        return FALSE;
    }

    //set txt to be sent and length of it
    error = libgnokii_bacatext_api(&sms.user_data[pos],unit);
    sms.user_data[pos].type = GN_SMS_DATA_Text;
/*
        if ((sms.dcs.u.general.alphabet != GN_SMS_DCS_8bit) && !gn_char_def_alphabet(sms.user_data[pos].u.text))
		sms.dcs.u.general.alphabet = GN_SMS_DCS_UCS2;
		sms.user_data[++pos].type = GN_SMS_DATA_None;

*/

    //report the delivery
    sms.delivery_report = 1;

    //auto get sms center
    data.message_center      = calloc(1, sizeof(gn_sms_message_center)); //allocate only for one element of data->message_center and size of its element is sizeof(gn_sms_message_center)
    data.message_center->id  = 1;

    error = gn_sm_functions(GN_OP_GetSMSCenter, &data, state);
    if(error == GN_ERR_NONE)
    {
        snprintf(sms.smsc.number,sizeof(sms.smsc.number),"%s",data.message_center->smsc.number); // set to sms.smsc.number from data.sms.smsc.number
        sms.smsc.type = data.message_center->smsc.type;
        //g_slice_free(gn_sms_message_center,data->message_center); // free the ram
        free(data.message_center);
    }
    if(!sms.smsc.number[0])
    {
        printf("failed once getting sms center number\n");
        return FALSE;
    }
    if(!sms.smsc.type)
    {
        sms.smsc.type = GN_GSM_NUMBER_Unknown;
    }

    //now give the data.sms from what all we have in sms, and make sure before these data.sms has been freed...
    data.sms = &sms;

    //send the message
    error = gn_sms_send(&data,state);

    if(error == GN_ERR_NONE)
    {
        if(sms.parts > 1)
        {
            int j;
            printf("sms sent with : %d parts, and reference number is : ", sms.parts);

            for(j=0; j < sms.parts; j++)
            {
                printf("%d\n",sms.reference[j]);
            }
        }
        else
        {
            printf("one sms sent with reference number : %d\n",sms.reference[0]);
        }
    }
    else
    {
        printf("libgnokii error : %s\n",gn_error_print(error));
        return FALSE;
    }

    free(sms.reference);

    return TRUE;
}

gboolean libgnokii_signal_info_api(pegang *unit)
{


      gn_data data;
      gn_error error;
   //prepare a place on memory
      gn_data_clear(&data);

      //sinyalling
      //paramter requirement of GN_OP_GetRFLevel
      printf("initiating get antena sinyal...\n");
      gn_rf_unit rfunit = GN_RF_Percentage;
      float rflevel;

      data.rf_unit = &rfunit;
      data.rf_level = &rflevel;

      if((error = gn_sm_functions(GN_OP_GetRFLevel, &data, state)) == GN_ERR_NONE)
      {
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(unit->info_dialog_main->progressbar_sinyal),(float)rflevel/100);
            printf("sinyal = %f\n",(float)rflevel/100);
            return TRUE;
      }
      else
      {

            down(unit);
            return FALSE;
      }
}



  gboolean libgnokii_closing(pegang *unit)
  {
      gn_lib_phone_close(state);
      gn_lib_phoneprofile_free(&state);
      gn_lib_library_free();


  }




gboolean sms_gateway_sender_api(pegang *unit)
{
    //overseas var
    gn_error error;



            if(!holdgnokii_init)
            {
                printf("setting file : %s\n",unit->info_dialog_main->file_setting);

                if(error = gn_lib_phoneprofile_load_from_file(unit->info_dialog_main->file_setting,NULL,&state) == GN_ERR_NONE)
                {
                    gn_lib_phone_open(state);
                    holdgnokii_init = TRUE;

                    printf("libgnokii initiated, and phone connected..\n");
                }
                else
                {

                    exit(-1);
                }
            }




                 if(libgnokii_signal_info_api(unit) == FALSE)
                 {
                    printf("libgnokii_signal_info_api(); is failed\n");
                    //return FALSE;
                 }


}

