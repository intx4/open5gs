#define TRACE_MODULE _s1enbmsg

#include "core_debug.h"
#include "core_pkbuf.h"
#include "core_lib.h"

#include "mme_context.h"
#include "s1ap_build.h"
#include "s1ap_conv.h"
#include "s1ap_path.h"

net_sock_t *tests1ap_enb_connect(void)
{
    char buf[INET_ADDRSTRLEN];
    status_t rv;
    mme_context_t *mme = mme_self();
    net_sock_t *sock = NULL;

    if (!mme) return NULL;

    rv = net_open_ext(&sock, mme->s1ap_addr, 
            INET_NTOP(&mme->s1ap_addr, buf), 0, mme->s1ap_port, 
            SOCK_SEQPACKET, IPPROTO_SCTP, SCTP_S1AP_PPID, 0);
    if (rv != CORE_OK) return NULL;

    return sock;
}

status_t tests1ap_enb_close(net_sock_t *sock)
{
    return net_close(sock);
}

int tests1ap_enb_send(net_sock_t *sock, pkbuf_t *sendbuf)
{
    return s1ap_send(sock, sendbuf);
}

int tests1ap_enb_read(net_sock_t *sock, pkbuf_t *recvbuf)
{
    int rc = 0;

    while(1)
    {
        rc = net_read(sock, recvbuf->payload, recvbuf->len, 0);
        if (rc == -2) 
        {
            continue;
        }
        else if (rc <= 0)
        {
            if (sock->sndrcv_errno == EAGAIN)
            {
                continue;
            }
            break;
        }
        else
        {
            break;
        }
    }

    return rc;
}


status_t tests1ap_build_setup_req(pkbuf_t **pkbuf, c_uint32_t enb_id)
{
    int erval = -1;

    s1ap_message_t message;
    S1ap_S1SetupRequestIEs_t *ies;
    S1ap_PLMNidentity_t *plmnIdentity;
    S1ap_SupportedTAs_Item_t *supportedTA;

    memset(&message, 0, sizeof(s1ap_message_t));

    ies = &message.s1ap_S1SetupRequestIEs;

    s1ap_uint32_to_ENB_ID(S1ap_ENB_ID_PR_macroENB_ID, enb_id, 
        &ies->global_ENB_ID.eNB_ID);
    s1ap_buffer_to_OCTET_STRING(&mme_self()->served_tai[0].plmn_id,
            PLMN_ID_LEN, &ies->global_ENB_ID.pLMNidentity);

    supportedTA = (S1ap_SupportedTAs_Item_t *)
        core_calloc(1, sizeof(S1ap_SupportedTAs_Item_t));
    s1ap_uint16_to_OCTET_STRING(
            mme_self()->served_tai[0].tac, &supportedTA->tAC);
    plmnIdentity = (S1ap_PLMNidentity_t *)
        core_calloc(1, sizeof(S1ap_PLMNidentity_t));
    s1ap_buffer_to_OCTET_STRING(&mme_self()->served_tai[0].plmn_id,
            PLMN_ID_LEN, plmnIdentity);
    ASN_SEQUENCE_ADD(&supportedTA->broadcastPLMNs, plmnIdentity);

    ASN_SEQUENCE_ADD(&ies->supportedTAs, supportedTA);

    ies->defaultPagingDRX = S1ap_PagingDRX_v64;

    message.direction = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = S1ap_ProcedureCode_id_S1Setup;

    erval = s1ap_encode_pdu(pkbuf, &message);
    s1ap_free_pdu(&message);

    if (erval < 0)
    {
        d_error("s1ap_encode_error : (%d)", erval);
        return CORE_ERROR;
    }

    return CORE_OK;
}

#define TESTS1AP_MAX_MESSAGE 10

status_t tests1ap_build_initial_ue_msg(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] = {
        "000c405800000500 0800020001001a00 302f177ca0b38802 0741020809101010"
        "3254869104e060c0 4000050221d011d1 5c0a003103e5e034 9011035758a65d01"
        "00004300060000f1 105ba00064400800 00f1101079baf000 86400130",

        "000c406800000500 080002001f001a00 403f074172080910 10103254866202e0"
        "600021023cd011d1 271a808021100100 0010810600000000 830600000000000d"
        "00000a005c0a0090 11034f18a6f15d01 00004300060000f1 1030390064400800"
        "00f110002343d000 86400130",

        "000c"
        "404c000005000800 020002001a002423 0741710809101010 3254767905f0f000"
        "0000000e0201d011 d1270780000a0000 0d00c10043000600 00f1102b67006440"
        "080000f11054f640 100086400130",
    };
    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        92,
        108,
        80,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

status_t tests1ap_build_authentication_response(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] = {
        "000d403e00000500 000005c00100009d 000800020001001a 001211177c0bca9d"
        "030753086a91970e 838fd07900644008 0000f1101079baf0 004340060000f110"
        "5ba0",

        "000d403500000500 0000020001000800 02001f001a000c0b 07530831c964f076"
        "1378760064400800 00f110002343d000 4340060000f11030 39",

        "000d"
        "4038000005000000 05c0020000c80008 00020002001a000c 0b0753087dc78e7c"
        "421f9eb900644008 0000f11054f64010 004340060000f110 2b67",
    };

    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        66,
        57,
        60,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

status_t tests1ap_build_security_mode_complete(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] = {
        "000d403500000500 000005c00100009d 000800020001001a 000908476b8f5f64"
        "00075e0064400800 00f1101079baf000 4340060000f1105b a0",

        "000d403200000500 0000020001000800 02001f001a000908 473c0c819e00075e"
        "006440080000f110 002343d000434006 0000f1103039",

        "000d"
        "4035000005000000 05c0020000c80008 00020002001a0009 0847c0eb1eb80007"
        "5e006440080000f1 1054f64010004340 060000f1102b67",

    };
    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        57,
        54,
        57,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

status_t tests1ap_build_esm_information_response(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] =  {
        "000d40808d000005 00000005c0010000 9d00080002000100 1a006160275c0667"
        "58010221da280908 696e7465726e6574 274a80c223150100 001510c09a2626c0"
        "9a2626c09a2626c0 9a2626c223150200 0015103d3dda5c72 4cc497354ae64653"
        "45a8088021100100 0010810600000000 830600000000000d 00000a0000644008"
        "0000f1101079baf0 004340060000f110 5ba0",

        "000d403e00000500 0000020001000800 02001f001a001514 27505a0b5301023c"
        "da280908696e7465 726e657400644008 0000f110002343d0 004340060000f110"
        "3039",

        "000d"
        "4041000005000000 05c0020000c80008 00020002001a0015 142793b2bedc0102"
        "01da280908696e74 65726e6574006440 080000f11054f640 10004340060000f1"
        "102b67"
    };
    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        146,
        66,
        69,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

status_t tests1ap_build_ue_capability_info_indication(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] = {
        "0016"
        "40809c0000030000 0005c00100009d00 0800020001004a40 8085808304040003"
        "04ec598007000820 81839b4e1e3ff8ff f1ffc7ff8ffe3ffc 7ff1ffe3ff8fff1f"
        "fc7ff8ffe3ffc7ff 1ffdfd3ffa7a2060 090e194e9525c8c2 fd80000000e03fe7"
        "ff5f60000000384f e9ffd3d800000002 1033035758a66014 042f6513b8800d2f"
        "0831c1a53432b259 ef989007000cdd9c 6331200e0019a332 c662401c003200",

        "0016402d00000300 0000020001000800 02001f004a401a19 00b801014c598080"
        "9c000bf06ec4d001 40302c0000000000 000000",

        "0016"
        "403f000003000000 05c0020000c80008 00020002004a4029 28013001023cd980"
        "00bc000ff06ec4d0 0141b82c00000000 07d404000ef08020 00012a0300008800"
        "00",
    };
    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        161,
        51,
        67,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

status_t tests1ap_build_initial_context_setup_response(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] = { 
        "2009"
        "0025000003000040 05c00100009d0008 400200010033400f 000032400a0a1f0a"
        "012d2801000008",

        "2009002200000300 0040020001000840 02001f0033400f00 0032400a0a1f0a01"
        "23c501000508",

        "2009"
        "0025000003000040 05c0020000c80008 400200020033400f 000032400a0a1f0a"
        "0123a701000008",

    };
    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        41,
        38,
        41,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

status_t tests1ap_build_attach_complete(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] = {
        "000d"
        "403a000005000000 05c00100009d0008 00020001001a000e 0d27c183eb950207"
        "4300035200c20064 40080000f1101079 baf0004340060000 f1105ba0",
        "",

        "000d"
        "403a000005000000 05c0020000c80008 00020002001a000e 0d27f190fc2b0207"
        "4300035200c20064 40080000f11054f6 4010004340060000 f1102b67",
    };
    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        62,
        0,
        62,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

status_t tests1ap_build_emm_status(pkbuf_t **pkbuf, int i)
{
    char *payload[TESTS1AP_MAX_MESSAGE] = { 
        "",

        "000d403300000500 0000020001000800 02001f001a000a09 27574292cc020760"
        "65006440080000f1 10002343d0004340 060000f1103039",
    };
    c_uint16_t len[TESTS1AP_MAX_MESSAGE] = {
        0,
        55,
    };
    char hexbuf[MAX_SDU_LEN];
    
    *pkbuf = pkbuf_alloc(0, MAX_SDU_LEN);
    if (!(*pkbuf)) return CORE_ERROR;

    (*pkbuf)->len = len[i];
    memcpy((*pkbuf)->payload, CORE_HEX(payload[i], strlen(payload[i]), hexbuf),
            (*pkbuf)->len);

    return CORE_OK;
}

