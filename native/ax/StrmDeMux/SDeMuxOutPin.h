/*
 * Copyright 2015 The SageTV Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __DEMUXOUTPUTPIN_H__
#define __DEMUXOUTPUTPIN_H__

#include <stdio.h>
#include "ISDeMux.h"
#include "SDeMuxThread.h"


#include "../../../third_party/Microsoft/StrmDeMux/alloc.h"

#define SAFE_RELEASE(pObject) if(pObject){ pObject->Release(); pObject = NULL;}
#define SAFE_DELETE(pObject)  if(pObject){ delete pObject;     pObject = NULL;}

class CSDeMuxOutPin : public CBaseOutputPin
{
	friend class CSDeMuxFilter;
public:
    CSDeMuxOutPin( CSDeMuxFilter *pFilter, HRESULT *phr, LPCWSTR pName, 
		             CMediaType *pmt, BYTE uContentType, BYTE uStreamId, BYTE uSubId, BYTE uTrackIndex );
	~CSDeMuxOutPin( );

		// Override to expose IMediaSeeking
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppvoid );
	STDMETHODIMP_(ULONG) NonDelegatingAddRef() {  return CUnknown::NonDelegatingAddRef(); };
	STDMETHODIMP_(ULONG) NonDelegatingRelease() { return CUnknown::NonDelegatingRelease(); };

	BOOL	FirstSample(){ return m_bFirstSample; }
	HRESULT DeliverNewSegment( REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, double rate );
	HRESULT DeliverEndOfStream();
	HRESULT DeliverBeginFlush();
    HRESULT DeliverEndFlush();
	BOOL	QueueEmpty(); 
	void    SendAnyway();
    STDMETHODIMP Connect( IPin * pReceivePin, const AM_MEDIA_TYPE *pmt );
    

	// Send a completed sample generated by parser
	HRESULT SendSample( const BYTE *pbData, LONG lData, REFERENCE_TIME rtStart, REFERENCE_TIME rtStop, int nbSync );

	// Send a transformated sample generated by parser
	HRESULT OpenSample( REFERENCE_TIME rtStart, BOOL bSync );
	HRESULT LoadSampleData( const BYTE *pbData, LONG lData );
	void    DropSampleData( LONG lData );
	HRESULT SendSample( );
	HRESULT ResetSample( REFERENCE_TIME rtStart, BOOL bSync );
	HRESULT GetSampleTime( REFERENCE_TIME &rtStart, REFERENCE_TIME &rtStop );
	HRESULT SendPartialSample( const BYTE *pbData, LONG lData );
	void    DropSample( );

	BOOL    IsVideoPin() { return m_uContentType == 1; }; 
	ULONG	OutSampleCount( ) { return m_dwSampleCount; };
	void	ResetSampleCount( ) { m_dwSampleCount = 0; };
	CMediaType* GetAcceptMediaType() { return  m_pMediaAccept; };

	//BYTE m_uStreamType;
	BYTE m_uContentType;
	BYTE m_uStreamId;
	BYTE m_uSubId;
	BYTE m_uTrackIndex;

private:
	virtual HRESULT DecideBufferSize( IMemAllocator * pAlloc, ALLOCATOR_PROPERTIES * pProp);
	virtual HRESULT DecideAllocator( IMemInputPin *pPin, IMemAllocator **ppAlloc );
	virtual HRESULT InitAllocator( CSequentialAllocator *pAllocator, IMemAllocator **ppAlloc );

	HRESULT CheckMediaType(const CMediaType *);
	HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);
	HRESULT GetMediaType(int iPosition,CMediaType **pMediaType);
    HRESULT AddMediaType( CMediaType const *pmt  );
	CGenericList<CMediaType> m_lMedia;
	CMediaType *m_pMediaAccept;

    HRESULT Active(void);    // Starts up the worker thread
    HRESULT Inactive(void);  // Exits the worker thread.
	HRESULT CompleteConnect(IPin *pReceivePin);
	HRESULT BreakConnect(void);

	HRESULT FillBuffer( IMediaSample *pSample );
	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
    {
		DbgLog((LOG_TRACE, 1, TEXT("Notified Quality Mesage %s "), q.Type == Famine ? "Famine" : "Flood" ));
        return E_FAIL;
    }

	CSubAllocator *Allocator()
    {
        return (CSubAllocator *)m_pAllocator;
    }

	CSDeMuxFilter*	m_pFilter;
	COutputQueue   *m_pOutputQueue;
	CCritSec		m_cSharedState;
	ULONG			m_dwSampleCount;
	DWORD			m_dwBlockSize;
	BOOL			m_bFirstSample;
	BOOL			m_bDiscontinuity;


	IMediaSample  *m_pSample;
	LONG m_lSampleDataLength;

	//for debug 
	FILE* fd;
	void open_dump();
	void close_dump();
	void rewind_dump( );
	void dump_sample( IMediaSample *pSample );
	void open_dump(int index );
	int  _file_index;

};



#endif