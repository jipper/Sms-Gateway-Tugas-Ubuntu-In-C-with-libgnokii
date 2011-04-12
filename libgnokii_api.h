/* 
 * File:   libgnokii_api.h
 * Author: jipper
 *
 * Created on April 12, 2011, 11:51 PM
 */

#ifndef LIBGNOKII_API_H
#define	LIBGNOKII_API_H

#ifdef	__cplusplus
extern "C" {
#endif

gboolean libgnokii_sms_send_api(pegang *unit);
gboolean libgnokii_signal_info_api(pegang *unit);
gboolean libgnokii_closing(pegang *unit);
gboolean sms_gateway_sender_api(pegang *unit);
#ifdef	__cplusplus
}
#endif

#endif	/* LIBGNOKII_API_H */

