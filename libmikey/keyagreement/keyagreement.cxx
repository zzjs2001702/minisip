/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien, Joachim Orrblad
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 * Authors: Erik Eliasson <eliasson@it.kth.se>
 *          Johan Bilien <jobi@via.ecp.fr>
 *	    Joachim Orrblad <joachim@orrblad.com>
*/


#include<config.h>
#include<libmikey/keyagreement.h>
#include<libmikey/MikeyPayloadSP.h>
#include<string.h>
#include<libmutil/hmac.h>

KeyAgreement::KeyAgreement():
	tgkPtr(NULL), tgkLengthValue(0),
	randPtr(NULL), randLengthValue(0),
	csbIdValue(0), 
	csIdMapPtr(NULL), nCsValue(0),
	initiatorDataPtr(NULL), responderDataPtr(NULL){
	//policy = list<Policy_type *>::list();
	kvPtr = new KeyValidityNull();

}

KeyAgreement::~KeyAgreement(){
	if( tgkPtr )
		delete [] tgkPtr;
	if( randPtr )
		delete [] randPtr;
	list<Policy_type *>::iterator i;
	for( i = policy.begin(); i != policy.end() ; i++ )
		delete *i;
	policy.clear();
}

unsigned int KeyAgreement::tgkLength(){
	return tgkLengthValue;
}

unsigned char * KeyAgreement::tgk(){
	return tgkPtr;
}

unsigned int KeyAgreement::randLength(){
	return randLengthValue;
}

unsigned char * KeyAgreement::rand(){
	return randPtr;
}

MRef<KeyValidity *> KeyAgreement::keyValidity(){
	return kvPtr;
}

void KeyAgreement::setKeyValidity( MRef<KeyValidity *> kv ){
	this->kvPtr = NULL;
	
	switch( kv->type() ){
		case KEYVALIDITY_NULL:
			this->kvPtr = new KeyValidityNull();
			break;
		case KEYVALIDITY_SPI:
			this->kvPtr = 
				new KeyValiditySPI( *(KeyValiditySPI *)(*kv) );
			break;
		case KEYVALIDITY_INTERVAL:
			this->kvPtr = new KeyValidityInterval( 
					*(KeyValidityInterval *)(*kv) );
			break;
		default:
			return;
	}
}

void KeyAgreement::setRand( unsigned char * rand, int randLengthValue ){
	this->randLengthValue = randLengthValue;

	if( this->randPtr )
		delete [] this->randPtr;

	this->randPtr = new unsigned char[ randLengthValue ];
	memcpy( this->randPtr, rand, randLengthValue );
}

/* Described in draft-ietf-msec-mikey-07.txt Section 4.1.2 */
void p( unsigned char * s, unsigned int sLength, 
        unsigned char * label, unsigned int labelLength,
	unsigned int m,
	unsigned char * output )
{
	unsigned int i;
	unsigned int hmac_output_length;
	byte_t * hmac_input = new byte_t[ labelLength + 20 ];

	/* initial step */
	hmac_sha1( s, sLength,
	      label, labelLength,
	      hmac_input, &hmac_output_length );
	assert( hmac_output_length == 20 );
	memcpy( &hmac_input[20], label, labelLength );

	hmac_sha1( s, sLength, 
	      hmac_input, labelLength + 20,
	      output, &hmac_output_length );
	assert( hmac_output_length == 20 );

	for( i = 2; i <= m ; i++ )
	{
		/* Update the first part of the hmac_input (A_i)
		 * with the MAC of the previous one (A_(i-1)) */
		hmac_sha1( s, sLength, 
		      hmac_input, 20,
		      hmac_input, &hmac_output_length );
		assert( hmac_output_length == 20 );
		
		hmac_sha1( s, sLength, 
	      	      hmac_input, labelLength + 20,
	      	      &output[ 20 * (i-1) ], &hmac_output_length );
		assert( hmac_output_length == 20 );
	}

	delete [] hmac_input;
}

/* Described in draft-ietf-msec-mikey-07.txt Section 4.1.3 */
void prf( unsigned char * inkey,  unsigned int inkeyLength,
	  unsigned char * label,  unsigned int labelLength,
	  unsigned char * outkey, unsigned int outkeyLength )
{
	unsigned int n;
	unsigned int m;
	unsigned int i;
	unsigned int j;
	unsigned char * p_output;
	n = inkeyLength / 64 + 1;
	m = outkeyLength / 20 + 1;
	
	p_output = new unsigned char[ m * 20 ];

	memset( outkey, 0, outkeyLength );
	for( i = 1; i <= n-1; i++ )
	{
		p( &inkey[ (i-1)*64 ], 64, label, labelLength, m, p_output );
		for( j = 0; j < outkeyLength; j++ )
		{
			outkey[j] ^= p_output[j];
		}
	}

	/* Last step */
	p( &inkey[ (n-1)*64 ], inkeyLength % 64, 
			label, labelLength, m, p_output );
	
	for( j = 0; j < outkeyLength; j++ )
	{
		outkey[j] ^= p_output[j];
	}
	delete [] p_output;
}

void KeyAgreement::keyDeriv( unsigned char csId, unsigned int csbIdValue,
			      unsigned char * inkey, unsigned int inkeyLength,
			      unsigned char * key, unsigned int keyLength ,
			      int type ){

	byte_t * label = new byte_t[4+4+1+randLengthValue];

	switch( type ){
		case KEY_DERIV_SALT:
			label[0] = 0x39;
			label[1] = 0xA2;
			label[2] = 0xC1;
			label[3] = 0x4B;
			break;
		case KEY_DERIV_TEK:
			label[0] = 0x2A;
			label[1] = 0xD0;
			label[2] = 0x1C;
			label[3] = 0x64;
			break;
		case KEY_DERIV_TRANS_ENCR:
			label[0] = 0x15;
			label[1] = 0x05;
			label[2] = 0x33;
			label[3] = 0xE1;
			break;
		case KEY_DERIV_TRANS_SALT:
			label[0] = 0x29;
			label[1] = 0xB8;
			label[2] = 0x89;
			label[3] = 0x16;
			break;
		case KEY_DERIV_TRANS_AUTH:
			label[0] = 0x2D;
			label[1] = 0x22;
			label[2] = 0xAC;
			label[3] = 0x75;
			break;
		case KEY_DERIV_ENCR:
			label[0] = 0x15;
			label[1] = 0x79;
			label[2] = 0x8C;
			label[3] = 0xEF;  
			break;
		case KEY_DERIV_AUTH:
			label[0] = 0x1B;
			label[1] = 0x5C;
			label[2] = 0x79;
			label[3] = 0x73;
			break;
	}
	
	label[4] = csId;
	
	label[5] = (unsigned char)((csbIdValue>>24) & 0xFF);
	label[6] = (unsigned char)((csbIdValue>>16) & 0xFF);
	label[7] = (unsigned char)((csbIdValue>>8) & 0xFF);
	label[8] = (unsigned char)(csbIdValue & 0xFF);
	memcpy( &label[9], randPtr, randLengthValue );
	prf( inkey, inkeyLength, label, 9 + randLengthValue, key, keyLength );

	delete [] label;
}

void KeyAgreement::genTek( unsigned char csId,
			    unsigned char * tek, unsigned int tekLength ){
	keyDeriv( csId, csbIdValue, tgkPtr, tgkLengthValue, 
			tek, tekLength, KEY_DERIV_TEK );
}

void KeyAgreement::genSalt( unsigned char csId,
			     unsigned char * salt, unsigned int saltLength ){
	keyDeriv( csId, csbIdValue, tgkPtr, tgkLengthValue, 
			salt, saltLength, KEY_DERIV_SALT );
}

void KeyAgreement::genEncr( unsigned char csId,
			     unsigned char * e_key, unsigned int e_keylen ){
	keyDeriv( csId, csbIdValue, tgkPtr, tgkLengthValue, 
			e_key, e_keylen, KEY_DERIV_ENCR );
}

void KeyAgreement::genAuth( unsigned char csId,
			     unsigned char * a_key, unsigned int a_keylen ){
	keyDeriv( csId, csbIdValue, tgkPtr, tgkLengthValue, 
			a_key, a_keylen, KEY_DERIV_AUTH );
}

unsigned int KeyAgreement::csbId(){
	return csbIdValue;
}

void KeyAgreement::setCsbId( unsigned int csbIdValue ){
	this->csbIdValue = csbIdValue;
}

void KeyAgreement::setTgk( unsigned char * tgk, unsigned int tgkLengthValue ){
	if( this->tgkPtr )
		delete [] this->tgkPtr;
	this->tgkLengthValue = tgkLengthValue;
	this->tgkPtr = new unsigned char[ tgkLengthValue ];
	memcpy( this->tgkPtr, tgk, tgkLengthValue );
}

void * KeyAgreement::initiatorData(){
	return initiatorDataPtr;
}

void KeyAgreement::setInitiatorData( void * data ){
	initiatorDataPtr = data;
}

void * KeyAgreement::responderData(){
	return responderDataPtr;
}

void KeyAgreement::setResponderData( void * data ){
	responderDataPtr = data;
}

string KeyAgreement::authError(){
	return authErrorValue;
}

void KeyAgreement::setAuthError( string error ){
	authErrorValue = error;
}

void KeyAgreement::setCsIdMap( MRef<MikeyCsIdMap *> idMap ){
	csIdMapPtr = idMap;
}

MRef<MikeyCsIdMap *> KeyAgreement::csIdMap(){
	return csIdMapPtr;
}

byte_t KeyAgreement::nCs(){
	return nCsValue;
}

void KeyAgreement::setnCs(uint8_t value){
	nCsValue = value;
}

byte_t KeyAgreement::getSrtpCsId( uint32_t ssrc ){
	MikeyCsIdMapSrtp * csIdMap = 
		dynamic_cast<MikeyCsIdMapSrtp *>( *csIdMapPtr );

	if( csIdMap == NULL ){
		return 0;
	}

	return csIdMap->findCsId( ssrc );
}

uint32_t KeyAgreement::getSrtpRoc( uint32_t ssrc ){
	MikeyCsIdMapSrtp * csIdMap = 
		dynamic_cast<MikeyCsIdMapSrtp *>( *csIdMapPtr );

	if( csIdMap == NULL ){
		return 0;
	}
	return csIdMap->findRoc( ssrc );
}

uint8_t KeyAgreement::findpolicyNo( uint32_t ssrc ){
	MikeyCsIdMapSrtp * csIdMap = 
		dynamic_cast<MikeyCsIdMapSrtp *>( *csIdMapPtr );
	if( csIdMap == NULL ){
		return 0;
	}
	return csIdMap->findRoc( ssrc );
}

void KeyAgreement::addSrtpStream( uint32_t ssrc, uint32_t roc, byte_t policyNo, byte_t csId ){
	MikeyCsIdMapSrtp * csIdMap = 
		dynamic_cast<MikeyCsIdMapSrtp *>( *csIdMapPtr );

	if( csIdMap == NULL ){
		csIdMapPtr = new MikeyCsIdMapSrtp();
		csIdMap = (MikeyCsIdMapSrtp *)(*csIdMapPtr);
	}
	
	csIdMap->addStream( ssrc, roc, policyNo, csId );

	if( csId == 0 )
		nCsValue ++;
}

void KeyAgreement::addIpsecSA( uint32_t spi, uint32_t spiSrcaddr, uint32_t spiDstaddr, byte_t policyNo, byte_t csId){
	MikeyCsIdMapIPSEC4 * csIdMap = dynamic_cast<MikeyCsIdMapIPSEC4 *>( *csIdMapPtr );
	if( csIdMap == NULL ){
		csIdMapPtr = new MikeyCsIdMapIPSEC4();
		csIdMap = (MikeyCsIdMapIPSEC4 *)(*csIdMapPtr);
	}
	csIdMap->addSA(spi, spiSrcaddr, spiDstaddr, policyNo, csId);
	if( csId == 0 )
		nCsValue ++;
}

void KeyAgreement::setCsIdMapType(uint8_t type){
	CsIdMapType = type;
}
uint8_t KeyAgreement::getCsIdMapType(){
	return CsIdMapType;
}

/* Security Policy */
		 	
void KeyAgreement::setPolicyParamType(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type, uint16_t length, byte_t * value){
	Policy_type * pol;
	if ( (pol = getPolicyParamType(policy_No, prot_type, policy_type) ) == NULL)
		policy.push_back (new Policy_type(policy_No, prot_type, policy_type, length, value));
	else {
		policy.remove(pol);
		delete pol;
		policy.push_back (new Policy_type(policy_No, prot_type, policy_type, length, value));
	}
}

uint8_t KeyAgreement::setPolicyParamType(uint8_t prot_type, uint8_t policy_type, uint16_t length, byte_t * value){
	list<Policy_type *>::iterator i;
	uint8_t policyNo = 0;
	i = policy.begin();
	while( i != policy.end() ){
		if( (*i)->policy_No == policyNo ){
			i = policy.begin();
			policyNo++;
		}
		else
			i++;
	}
	policy.push_back (new Policy_type(policyNo, prot_type, policy_type, length, value));
	return policyNo;
}

uint8_t KeyAgreement::setdefaultPolicy(uint8_t prot_type){
	list<Policy_type *>::iterator iter;
	uint8_t policyNo = 0;
	iter = policy.begin();
	while( iter != policy.end() ){
		if( (*iter)->policy_No == policyNo ){
			iter = policy.begin();
			policyNo++;
		}
		else
			iter++;
	}
	byte_t ipsec4values[] = {MIKEY_IPSEC_SATYPE_ESP,MIKEY_IPSEC_MODE_TRANSPORT,MIKEY_IPSEC_SAFLAG_PSEQ,MIKEY_IPSEC_EALG_3DESCBC,24,MIKEY_IPSEC_AALG_SHA1HMAC,16};
	byte_t srtpvalues[] ={MIKEY_SRTP_EALG_AESCM,16,MIKEY_SRTP_AALG_SHA1HMAC,94,14,MIKEY_SRTP_PRF_AESCM,0,1,1,MIKEY_FEC_ORDER_FEC_SRTP,1,4,0};
	int i, arraysize;
	switch (prot_type) {
	case MIKEY_PROTO_SRTP:
		arraysize = 13;
		for(i=0; i< arraysize; i++)
			policy.push_back (new Policy_type(policyNo, prot_type, i, 1, &srtpvalues[i]));
		break;
	case MIKEY_PROTO_IPSEC4:
		arraysize = 7;
		for(i=0; i< arraysize; i++)
			policy.push_back (new Policy_type(policyNo, prot_type, i, 1, &ipsec4values[i]));
		break;
	}
	return policyNo;
}
	
Policy_type * KeyAgreement::getPolicyParamType(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type){
	list<Policy_type *>::iterator i;
	for( i = policy.begin(); i != policy.end()  ; i++ )
		if( (*i)->policy_No == policy_No && (*i)->prot_type == prot_type && (*i)->policy_type == policy_type )
			return *i;
	return NULL;
}

uint8_t KeyAgreement::getPolicyParamTypeValue(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type){
	list<Policy_type *>::iterator i;
	for( i = policy.begin(); i != policy.end()  ; i++ )
		if( (*i)->policy_No == policy_No && (*i)->prot_type == prot_type && (*i)->policy_type == policy_type && (*i)->length == 1)
			return (uint8_t)(*i)->value[0];
	return 0;
}

Policy_type::Policy_type(uint8_t policy_No, uint8_t prot_type, uint8_t policy_type, uint16_t length, byte_t * value){
	this->policy_No = policy_No;
	this->prot_type = prot_type;
	this->policy_type = policy_type;
	this->length = length;
	this->value = (byte_t*) calloc (length,sizeof(byte_t));
	for(int i=0; i< length; i++)
			this->value[i] = value[i];
}

Policy_type::~Policy_type(){
	free(value);
}







