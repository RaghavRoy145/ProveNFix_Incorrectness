typedef struct ssl_connection_st {
    int options;
   
} SSL_CONNECTION;

#define SEQ_NUM_SIZE                            8

#define SSL_OP_BIT(n)  ((int)1 << (int)n)

/*
 * SSL/TLS connection options.
 */
    /* Disable Extended master secret */
    /* Cleanse plaintext copies of data delivered to the application */
# define SSL_OP_CLEANSE_PLAINTEXT                        SSL_OP_BIT(1)


typedef struct tls_record_st {
    void *rechandle;
    int version;
    int type;
    /* The data buffer containing bytes from the record */
    unsigned char *data;
    /* Number of remaining to be read in the data buffer */
    int length;
    /* Offset into the data buffer where to start reading */
    int off;
    /* epoch number. DTLS only */
    int epoch;
    /* sequence number. DTLS only */
    unsigned char seq_num[SEQ_NUM_SIZE];
} TLS_RECORD;

int insert_ssl_release_record() 
/*@ insert_ssl_release_record: 
    Require TRUE, 𝝐
    Ensure  TRUE, ssl_release_record  @*/

{
    ssl_release_record(s, rr); 
}

int ssl3_read_bytes(int type, int *recvd_type, unsigned char *buf,
                    int len, int peek, int n)
/*@ ssl3_read_bytes: 
    Require TRUE, 𝝐
    Ensure  (peek=1, memcpy ·  ssl_release_record )  \/ 
           (!(peek=1), memcpy · (OPENSSL_cleanse \/  𝝐)  · (ssl_release_record \/  𝝐))  @*/
{
    TLS_RECORD *rr;
    SSL_CONNECTION *s;
    int totalbytes = 0;
    int curr_rec;
                n = len - totalbytes;

            memcpy(buf, &(rr->data[rr->off]), n);
            buf += n;
// previous: !peek /\ ELSE 
// expected: (peek /\ IF)  \/ (!peek /\ ELSE)
        //- if (!peek) {
            if (peek==1) {                         //+ 
                /* Mark any zero length record as consumed CVE-2016-6305 */
                //if (rr->length == 0)            //+
                   // ssl_release_record(s, rr);  //+
            } else {                            //+
                if (s->options & SSL_OP_CLEANSE_PLAINTEXT)
                    OPENSSL_cleanse(&(rr->data[rr->off]), n);
                rr->length -= n;
                rr->off += n;
                if (rr->length == 0)
                    ssl_release_record(s, rr);
            }
           /* if (rr->length == 0
                || (peek && n == rr->length)) {
                rr++;
                curr_rec++;
            }
            totalbytes += n;*/
}