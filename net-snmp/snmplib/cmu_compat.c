
#ifdef CMU_COMPATIBLE

#include <net-snmp/mib_api.h>
#include <net-snmp/pdu_api.h>
#include <net-snmp/session_api.h>

int mib_TxtToOid(char *Buf, oid **OidP, size_t *LenP)
{
    return read_objid(Buf, *OidP, LenP);
}

int mib_OidToTxt(oid *O, size_t OidLen, char *Buf, size_t BufLen)
{
    _sprint_objid(Buf, O, OidLen);
    return 1;
}



char *
snmp_pdu_type(struct snmp_pdu *PDU)
{
  switch(PDU->command) {
  case SNMP_MSG_GET:
    return("GET");
    break;
  case SNMP_MSG_GETNEXT:
    return("GETNEXT");
    break;
  case SNMP_MSG_RESPONSE:
    return("RESPONSE");
    break;
  case SNMP_MSG_SET:
    return("SET");
    break;
  case SNMP_MSG_GETBULK:
    return("GETBULK");
    break;
  case SNMP_MSG_INFORM:
    return("INFORM");
    break;
  case SNMP_MSG_TRAP2:
    return("V2TRAP");
    break;
  case SNMP_MSG_REPORT:
    return("REPORT");
    break;
    
  case SNMP_MSG_TRAP:
    return("V1TRAP");
    break;
  default:
    return("Unknown");
    break;
  }
}

/*
 * cmu_snmp_parse - emulate CMU library's snmp_parse.
 *
 * Parse packet, storing results into PDU.
 * Returns community string if success, NULL if fail.
 * WARNING: may return a zero length community string.
 *
 * Note:
 * Some CMU-aware apps call init_mib(), but do not
 * initialize a session.
 * Check Reqid to make sure that this module is initialized.
 */

u_char *
cmu_snmp_parse (struct snmp_session *session,
    struct snmp_pdu *pdu,
    u_char *data,
    size_t length)
{
    u_char *bufp = NULL;

    if (Reqid == 0) {
	snmp_sess_init(session); /* gimme a break! */
    }

    switch(pdu->version) {
    case SNMP_VERSION_1:
    case SNMP_VERSION_2c:
    case SNMP_DEFAULT_VERSION:
	    break;
    default:
	    return NULL;
    }
#ifndef NO_INTERNAL_VARLIST
    if (snmp_parse( 0, session, pdu, data, length) != SNMP_ERR_NOERROR){
	return NULL;
    }
#else
/*
 * while there are two versions of variable_list:
 * use an internal variable list for snmp_parse;
 * clone the result.
 */
if (1) {
struct snmp_pdu *snmp_clone_pdu (struct snmp_pdu *);
struct snmp_pdu *snmp_2clone_pdu(struct snmp_pdu *from_pdu, struct snmp_pdu *to_pdu);

    struct snmp_pdu *ipdu;
    ipdu = snmp_clone_pdu(pdu);
    if (snmp_parse( 0, session, ipdu, data, length) != SNMP_ERR_NOERROR){
	snmp_free_internal_pdu(ipdu);
	return NULL;
    }
    pdu = snmp_2clone_pdu(ipdu, pdu);
    snmp_free_internal_pdu(ipdu);
}
#endif /* NO_INTERNAL_VAR_LIST */

    /* Add a null to meet the caller's expectations. */

    bufp = (u_char *)malloc(1+pdu->community_len);
    if (bufp && pdu->community_len) {
	memcpy(bufp, pdu->community, pdu->community_len);
	bufp[pdu->community_len] = '\0';
    }
    return(bufp);
}


#endif /* CMU_COMPATIBLE */
