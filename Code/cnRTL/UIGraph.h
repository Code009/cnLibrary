/*- cnRTL - UI Graph ------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2009-03-22                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnRTL_UIGraph_H__
#define __cnLibrary_cnRTL_UIGraph_H__
/*-------------------------------------------------------------------------*/
#include <cnRTL/cnRTLHeader.h>
#include <cnRTL/RuntimeFunction.h>
#include <cnRTL/ObjectReference.h>
#include <cnRTL/InterfaceImplementation.h>

#ifdef __cplusplus
//---------------------------------------------------------------------------
#if cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#define	cnRTL_CONSTEXPR_FUNC	constexpr
#define	cnRTL_CONSTVAR	constexpr
// cnLibrary_CPPFEATURE_CONSTEXPR >= 200704L
#else
// cnLibrary_CPPFEATURE_CONSTEXPR < 200704L
#define	cnRTL_CONSTEXPR_FUNC
#define	cnRTL_CONSTVAR	const
#endif // cnLibrary_CPPFEATURE_CONSTEXPR

#if	cnLibrary_CPPFEATURE_IF_CONSTEXPR >= 201606L
#define	cnRTL_IFCONSTEXPR	constexpr
#else	// cnLibrary_CPPFEATURE_IF_CONSTEXPR < 201606L
#define	cnRTL_IFCONSTEXPR
#endif	// cnLibrary_CPPFEATURE_IF_CONSTEXPR

// PixelProcess
//---------------------------------------------------------------------------
namespace cnLibrary{

namespace cnRTL{

cnRTL_CONSTEXPR_FUNC cUIColor UIColorFromUInt32(uInt32 Value)noexcept(true){
	return {(Value&0xFF)/255.f,((Value>>8)&0xFF)/255.f,((Value>>16)&0xFF)/255.f,((Value>>24)&0xFF)/255.f};
}

#if cnLibrary_CPPFEATURE_USER_DEFINED_LITERALS >= 200809L
cnRTL_CONSTEXPR_FUNC cUIColor operator ""_cUIColor(unsigned long long int Value)noexcept(true){
	return UIColorFromUInt32(static_cast<uInt32>(Value));
}
#endif

//---------------------------------------------------------------------------
class cUIFontInfo : public iUIFont
{
public:
	void SetFontInfo(cString<uChar16> &&Name,eUIFontStyle FontStyle=UIFontStyle::Normal,Float32 Weight=0)noexcept(true);

	virtual rPtr< iArrayReference<const uChar16> > cnLib_FUNC GetName(void)noexcept(true)override;
	virtual eUIFontStyle cnLib_FUNC GetStyle(void)noexcept(true)override;

	virtual Float32 cnLib_FUNC GetWeight(void)noexcept(true)override;

private:
	cString<uChar16> fName;
	Float32 fWeight;
	eUIFontStyle fFontStyle;
};

//---------------------------------------------------------------------------
struct cSortableUIPoint : cUIPoint
{
	bool operator ==(const cUIPoint &Src)const noexcept(true);
	bool operator !=(const cUIPoint &Src)const noexcept(true);

	sfInt8 Compare(const cUIPoint &Cmp)const noexcept(true);
	cnLib_DEFINE_CLASS_THREE_WAY_COMPARISON(const cUIPoint &,Compare)
};
//---------------------------------------------------------------------------

#if 0

template<uIntn ElementCount,class TElement>
cNormIntVector<TElement,ElementCount> NormIntVectorLoad(const TElement *Value)
{
	cNormIntVector<TElement,ElementCount> RetValue;
	RetValue.Load(Value);
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
cNormIntVector<TElement,ElementCount> NormIntVectorLoadA(const TElement *Value)
{
	cNormIntVector<TElement,ElementCount> RetValue;
	RetValue.LoadA(Value);
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
cNormIntVector<TElement,ElementCount> NormIntVectorLoad(const TElement *Value,uIntn Count)
{
	cNormIntVector<TElement,ElementCount> RetValue;
	RetValue.LoadN(Value,Count);
	return RetValue;
}
//---------------------------------------------------------------------------
template<class TElement,uIntn ElementCount>
cNormIntVector<TElement,ElementCount> MakeNormIntVectorZero(void)
{
	cNormIntVector<TElement,ElementCount> RetValue;
	RetValue.Zero();
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
cNormIntVector<TElement,ElementCount> MakeNormIntVectorFill(TElement Value)
{
	cNormIntVector<TElement,ElementCount> RetValue;
	RetValue.Fill(Value);
	return RetValue;
}
//---------------------------------------------------------------------------
class cNormIntVectorZeroValue
{
public:
	template<class TElement,uIntn ElementCount>
	operator cNormIntVector<TElement,ElementCount> ()const{
		return MakeNormIntVectorZero<TElement,ElementCount>();
	}
};
extern const cNormIntVectorZeroValue NormIntVectorZeroValue;
//---------------------------------------------------------------------------
template<class TValue>
class cNormIntVectorFillValue
{
public:
	cNormIntVectorFillValue(TValue Value):fValue(Value){}

	template<class TElement,uIntn ElementCount>
	operator cNormIntVector<TElement,ElementCount> ()const{
		return MakeNormIntVectorFill<ElementCount>(static_cast<TElement>(fValue));
	}
private:
	TValue fValue;
};

template<class TElement>
cNormIntVectorFillValue<TElement> NormIntVectorFillValue(TElement Value)
{
	return Value;
}

//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
cColorVector<TElement,ElementCount> ColorVectorLoad(const TElement *Value)
{
	cColorVector<TElement,ElementCount> RetValue;
	for(uIntn i=0;i<ElementCount;i++){
		RetValue[i]=Value[i];
	}
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
cColorVector<TElement,ElementCount> ColorVectorLoad(const TElement *Value,uIntn Count)
{
	cColorVector<TElement,ElementCount> RetValue;
	for(uIntn i=0;i<Count;i++){
		RetValue[i]=Value[i];
	}
	return RetValue;
}
//---------------------------------------------------------------------------
template<class TElement,uIntn ElementCount>
cColorVector<TElement,ElementCount> MakeColorVectorZero(void)
{
	cColorVector<TElement,ElementCount> RetValue;
	RetValue.Zero();
	return RetValue;
}
//---------------------------------------------------------------------------
template<uIntn ElementCount,class TElement>
cColorVector<TElement,ElementCount> MakeColorVectorFill(TElement Value)
{
	cColorVector<TElement,ElementCount> RetValue;
	RetValue.Fill(Value);
	return RetValue;
}
//---------------------------------------------------------------------------
class cColorVectorZeroValue
{
public:
	template<class TElement,uIntn ElementCount>
	operator cColorVector<TElement,ElementCount> ()const{
		return MakeColorVectorZero<TElement,ElementCount>();
	}
};
extern const cColorVectorZeroValue ColorVectorZeroValue;
//---------------------------------------------------------------------------
template<class TValue>
class cColorVectorFillValue
{
public:
	cColorVectorFillValue(TValue Value):fValue(Value){}

	template<class TElement,uIntn ElementCount>
	operator cColorVector<TElement,ElementCount> ()const{
		return MakeColorVectorFill<ElementCount>(static_cast<TElement>(fValue));
	}
private:
	TValue fValue;
};

template<class TElement>
cColorVectorFillValue<TElement> ColorVectorFillValue(TElement Value)
{
	return Value;
}
#endif // 0

//---------------------------------------------------------------------------
template<class TElement>
cVector<uInt8,4> RGBAColorVectorTo8(cVector<TElement,4> &Color)noexcept(true);
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class iBitmapSampler : public iReference
{
public:
	virtual void Start(cUIPoint Size)noexcept(true)=0;
	virtual void Close(void)noexcept(true)=0;
	virtual cVector<TChannel,ChannelCount> Pixel(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class iBitmapPixelProcessor : public iReference
{
public:
	virtual void Start(cUIPoint Size)noexcept(true)=0;
	virtual void Close(void)noexcept(true)=0;
	virtual cVector<TChannel,ChannelCount> Pixel(cVector<TChannel,ChannelCount> Dest)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class iBitmapNormSampler : public iReference
{
public:
	virtual void Start(cUIPoint Size)noexcept(true)=0;
	virtual void Close(void)noexcept(true)=0;
	virtual cIntegerVector<TChannel,ChannelCount> Pixel(void)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class iBitmapNormPixelProcessor : public iReference
{
public:
	virtual void Start(cUIPoint Size)noexcept(true)=0;
	virtual void Close(void)noexcept(true)=0;
	virtual cIntegerVector<TChannel,ChannelCount> Pixel(cIntegerVector<TChannel,ChannelCount> Dest)noexcept(true)=0;
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class cBitmapNormRenderPipeline : public iBitmapNormSampler<TChannel,ChannelCount>
{
public:
	typedef cIntegerVector<TChannel,ChannelCount> cPixel;
	virtual void Start(cUIPoint Size)noexcept(true)override{
		SourceSampler->Start(Size);
		for(auto &Processor : PixelProcessors){
			Processor->Start(Size);
		}
	}
	virtual void Close(void)noexcept(true)override{
		SourceSampler->Close();
		for(auto &Processor : PixelProcessors){
			Processor->Close();
		}
	}
	virtual cPixel Pixel(void)noexcept(true)override{
		auto Dest=SourceSampler->Pixel();
		for(auto &Processor : PixelProcessors){
			Dest=Processor->Pixel(Dest);
		}
		return Dest;
	}


	rPtr< iBitmapNormSampler<TChannel,ChannelCount> > SourceSampler;
	cSeqList< rPtr< iBitmapNormPixelProcessor<TChannel,ChannelCount> > > PixelProcessors;
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount,cIntegerVector<TChannel,ChannelCount> PixelProcessor(cIntegerVector<TChannel,ChannelCount> Dest,cIntegerVector<TChannel,ChannelCount> Src)>
class cBitmapNormSampleBlendingProcessor : public iBitmapNormPixelProcessor<TChannel,ChannelCount>
{
public:
	typedef cIntegerVector<TChannel,ChannelCount> cPixel;
	virtual void Start(cUIPoint Size)noexcept(true)override{
		SourceSampler->Start(Size);
	}
	virtual void Close(void)noexcept(true)override{
		SourceSampler->Close();
	}
	virtual cPixel Pixel(cPixel Dest)noexcept(true)override{
		auto SrcPixel=SourceSampler->Pixel();
		return PixelProcessor(Dest,SrcPixel);
	}

	rPtr< iBitmapNormSampler<TChannel,ChannelCount> > SourceSampler;
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class cBitmapNormPixelProcessor_Noop : public iBitmapNormPixelProcessor<TChannel,ChannelCount>
{
public:
	virtual void Start(cUIPoint Size)noexcept(true)override{}
	virtual void Close(void)noexcept(true)override{}
	virtual cIntegerVector<TChannel,ChannelCount> Pixel(cIntegerVector<TChannel,ChannelCount> Dest)noexcept(true)override{
		return Dest;
	}
};
//---------------------------------------------------------------------------
template<class T,uIntn Count>
inline cIntegerVector<T,Count> BitmapNormPixelProcess_Copy(cIntegerVector<T,Count>,cIntegerVector<T,Count> Src)noexcept(true)
{	return Src;	}

template<class TChannel,uIntn ChannelCount>	using cBitmapNormSampleBlendingProcessor_Copy
	=cBitmapNormSampleBlendingProcessor<TChannel,ChannelCount,BitmapNormPixelProcess_Copy>;

//---------------------------------------------------------------------------

template<class T,uIntn Count>
inline cIntegerVector<T,Count> BitmapNormPixelProcess_Or(cIntegerVector<T,Count> Dest,cIntegerVector<T,Count> Src)noexcept(true)
{	return Dest | Src;	}

template<class T,uIntn Count>
inline cIntegerVector<T,Count> BitmapNormPixelProcess_And(cIntegerVector<T,Count> Dest,cIntegerVector<T,Count> Src)noexcept(true)
{	return Dest & Src;	}

template<class T,uIntn Count>
inline cIntegerVector<T,Count> BitmapNormPixelProcess_Xor(cIntegerVector<T,Count> Dest,cIntegerVector<T,Count> Src)noexcept(true)
{	return Dest ^ Src;	}

template<class TChannel,uIntn ChannelCount>	using cBitmapNormSampleBlendingProcessor_Or
	=cBitmapNormSampleBlendingProcessor<TChannel,ChannelCount,BitmapNormPixelProcess_Or>;
template<class TChannel,uIntn ChannelCount>	using cBitmapNormSampleBlendingProcessor_And
	=cBitmapNormSampleBlendingProcessor<TChannel,ChannelCount,BitmapNormPixelProcess_And>;
template<class TChannel,uIntn ChannelCount>	using cBitmapNormSampleBlendingProcessor_Xor
	=cBitmapNormSampleBlendingProcessor<TChannel,ChannelCount,BitmapNormPixelProcess_Xor>;


//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class cBitmapSourceNormSampler : public iBitmapNormSampler<TChannel,ChannelCount>
{
public:
	static cnRTL_CONSTVAR bool ChannelIsSigned=cnVar::TIsSigned<TChannel>::Value;

	cBitmapSourceNormSampler()noexcept(true);
	~cBitmapSourceNormSampler()noexcept(true);

	iPtr<iBitmapDataSource> Source;
	cFunction<iPtr<iBitmapDataSource> (cUIPoint Size)> QuerySource;

	rPtr< iBitmapNormPixelProcessor<TChannel,ChannelCount> > ColorConverter;

	virtual void Start(cUIPoint Size)noexcept(true)override{
		fSampleColorConverter=ColorConverter;
		if(fSampleColorConverter==nullptr){
			fSampleColorConverter=&fNullPixelProcessor;
		}
		fSampleSize=Size;
		auto SamplingSource=Source;
		if(SamplingSource==nullptr){
			if(QuerySource==nullptr){
				SetupZeroSampler();
				return;
			}
			SamplingSource=QuerySource(Size);
			if(SamplingSource==nullptr){
				SetupZeroSampler();
				return;
			}
		}
		fSourcePixelData=fSource->AcquirePixels();
		if(fSourcePixelData==nullptr){
			SetupZeroSampler();
			return;
		}

		auto SourcePixelFormat=fSource->GetPixelFormat();
		fSourceChannelCount=SourcePixelFormat.ChannelCount;
		fSourceChannelSize=SourcePixelFormat.ChannelSize;
		fSourcePitch=fSource->GetDataPitch();
		fSourceSize=fSource->GetImageSize();

		fSampleX=0;
		fSampleY=0;

		if(fSourceChannelCount>=ChannelCount){
			// load from vector with same channel count
		}
		else{
		}

		if(fSourceChannelCount==ChannelCount && fSourceChannelSize==sizeof(TChannel)){
			switch(SourcePixelFormat.ChannelType){
			case BitmapChannelType::UNorm:
				if(cnMemory::PointerIsAligned<cIntegerVector<TChannel,ChannelCount> >(fSourcePixelData)
					&& cnMemory::PointerIsAligned<cIntegerVector<TChannel,ChannelCount> >(fSourcePitch)
				){
					fPixelSampler=[this](const void *PixelData){
						return PixelSampler_MatchAlignedUNormT(PixelData);
					};
				}
				else{
					fPixelSampler=[this](const void *PixelData){
						return PixelSampler_MatchUNormT(PixelData);
					};
				}
				break;
			case BitmapChannelType::SNorm:
				if(cnMemory::PointerIsAligned<cIntegerVector<TChannel,ChannelCount> >(fSourcePixelData)
					&& cnMemory::PointerIsAligned<cIntegerVector<TChannel,ChannelCount> >(fSourcePitch)
				){
					fPixelSampler=[this](const void *PixelData){
						return PixelSampler_MatchAlignedSNormT(PixelData);
					};
				}
				else{
					fPixelSampler=[this](const void *PixelData){
						return PixelSampler_MatchSNormT(PixelData);
					};
				}
				break;
			case BitmapChannelType::Float:
			case BitmapChannelType::UIntegral:
			case BitmapChannelType::SIntegral:
				break;
			}
		}
		else{
		}
		if(fSourceChannelSize==sizeof(TChannel)){
			if(fSourceChannelCount==ChannelCount){
			}
			else{
			}
		}
		

		switch(SourcePixelFormat.ChannelType){
		default:
			SetupZeroSampler();
			break;
		case BitmapChannelType::UNorm:
			switch(SourcePixelFormat.ChannelSize){
			default:
				SetupZeroSampler();
				break;
			case 1:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_UNormT<uInt8>(PixelData);
				};
				break;
			case 2:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_UNormT<uInt16>(PixelData);
				};
				break;
			case 4:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_UNormT<uInt32>(PixelData);
				};
				break;
			}
			break;
		case BitmapChannelType::SNorm:
			switch(SourcePixelFormat.ChannelSize){
			default:
				SetupZeroSampler();
				break;
			case 1:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_SNormT<sInt8>(PixelData);
				};
				break;
			case 2:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_SNormT<sInt16>(PixelData);
				};
				break;
			case 4:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_SNormT<sInt32>(PixelData);
				};
				break;
			}
			break;
		case BitmapChannelType::Float:
			switch(SourcePixelFormat.ChannelSize){
			default:
				SetupZeroSampler();
				break;
			case 4:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_FloatT<Float32>(PixelData);
				};
				break;
			case 8:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_FloatT<Float64>(PixelData);
				};
				break;
			}
			break;
		case BitmapChannelType::SIntegral:
			switch(SourcePixelFormat.ChannelSize){
			default:
				SetupZeroSampler();
				break;
			case 1:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_SIntT<sInt8>(PixelData);
				};
				break;
			case 2:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_SIntT<sInt16>(PixelData);
				};
				break;
			case 4:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_SIntT<sInt32>(PixelData);
				};
				break;
			}
			break;
		case BitmapChannelType::UIntegral:
			switch(SourcePixelFormat.ChannelSize){
			default:
				SetupZeroSampler();
				break;
			case 1:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_UIntT<uInt8>(PixelData);
				};
				break;
			case 2:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_UIntT<uInt16>(PixelData);
				};
				break;
			case 4:
				fPixelSampler=[this](const void *PixelData){
					return PixelSampler_UIntT<uInt32>(PixelData);
				};
				break;
			}
			break;
		}
	}

	virtual void Close(void)noexcept(true)override{
		if(fSourcePixelData!=nullptr){
			fSource->ReleasePixels();
		}
	}

	virtual cIntegerVector<TChannel,ChannelCount> Pixel(void)noexcept(true)override{
		auto SamplePixel=fPixelSampler(fSourceScanLine);
		SamplePixel=fSampleColorConverter->Pixel(SamplePixel);

		fSourceScanPixel=cnMemory::PointerAddByteOffset(fSourceScanPixel,fSourcePixelSize);

		fSampleX++;
		if(fSampleX>=fSampleSize.x){
			fSampleY++;
			fSampleX=0;
			fSourceScanLine=cnMemory::PointerAddByteOffset(fSourceScanLine,fSourcePitch);
			fSourceScanPixel=fSourceScanLine;
		}
	}

	class cNullPixelProcessor : public iBitmapNormPixelProcessor<TChannel,ChannelCount>
	{
	public:
		virtual void Start(cUIPoint Size)noexcept(true)override{}
		virtual void Close(void)noexcept(true)override{}
		virtual cIntegerVector<TChannel,ChannelCount> Pixel(cIntegerVector<TChannel,ChannelCount> Dest)noexcept(true)override{
			return Dest;
		}
	};
protected:
	cUIPoint fSampleSize;
	iPtr<iBitmapDataSource> fSource;
	const void *fSourcePixelData;
	uIntn fSourcePitch;
	cUIPoint fSourceSize;
	ufInt8 fSourceChannelCount;
	ufInt8 fSourceChannelSize;
	ufInt8 fSourcePixelSize;
	const void *fSourceScanLine;
	const void *fSourceScanPixel;
	sfInt32 fSampleX;
	sfInt32 fSampleY;

	rPtr< iBitmapNormPixelProcessor<TChannel,ChannelCount> > fSampleColorConverter;
	cFunction<cIntegerVector<TChannel,ChannelCount> (const void *PixelData)> fPixelSampler;

	cnRTL::cReferenceStaticImplementT<cBitmapNormPixelProcessor_Noop<TChannel,ChannelCount> > fNullPixelProcessor;

	void SetupZeroSampler(void)noexcept(true){
		fPixelSampler=[this](const void*){
			return VectorZeroValue;
		};
	}

	cIntegerVector<TChannel,ChannelCount> PixelSampler_MatchUNormT(const void *PixelData)noexcept(true){
		if cnRTL_IFCONSTEXPR(cnVar::TIsSigned<TChannel>::Value){
			typedef typename cnVar::TIntegerOfSize<sizeof(TChannel),false>::Type TUNormInt;
			auto UNorm=IntegerVectorMake<ChannelCount>(static_cast<const TUNormInt*>(PixelData));
			UNorm=UNorm.Max(NormIntVectorFillValue(cnVar::TIntegerValue<TChannel>::Max));
			return reinterpret_cast<cIntegerVector<TChannel,ChannelCount>&>(UNorm);
		}
		else{
			return IntegerVectorMake<ChannelCount>(static_cast<const TChannel*>(PixelData));
		}
	}

	cIntegerVector<TChannel,ChannelCount> PixelSampler_MatchAlignedUNormT(const void *PixelData)noexcept(true){
		if cnRTL_IFCONSTEXPR(cnVar::TIsSigned<TChannel>::Value){
			typedef typename cnVar::TIntegerOfSize<sizeof(TChannel),false>::Type TUNormInt;
			auto UNorm=IntegerVectorMake<ChannelCount>(static_cast<const TUNormInt*>(PixelData));
			UNorm=UNorm.Max(NormIntVectorFillValue(cnVar::TIntegerValue<TChannel>::Max));
			return reinterpret_cast<cIntegerVector<TChannel,ChannelCount>&>(UNorm);
		}
		else{
			return IntegerVectorMake<ChannelCount>(static_cast<const TChannel*>(PixelData));
		}
	}

	cIntegerVector<TChannel,ChannelCount> PixelSampler_MatchSNormT(const void *PixelData)noexcept(true){
		if cnRTL_IFCONSTEXPR(cnVar::TIsSigned<TChannel>::Value){
			return IntegerVectorMake<ChannelCount>(static_cast<const TChannel*>(PixelData));
		}
		else{
			typedef typename cnVar::TIntegerOfSize<sizeof(TChannel),true>::Type TSNormInt;
			auto SNorm=IntegerVectorMake<ChannelCount>(static_cast<const TSNormInt*>(PixelData));
			SNorm=SNorm.Min(VectorZeroValue);
			return reinterpret_cast<cIntegerVector<TChannel,ChannelCount>&>(SNorm);
		}
	}

	cIntegerVector<TChannel,ChannelCount> PixelSampler_MatchAlignedSNormT(const void *PixelData)noexcept(true){
		if cnRTL_IFCONSTEXPR(cnVar::TIsSigned<TChannel>::Value){
			return IntegerVectorMake<ChannelCount>(static_cast<const TChannel*>(PixelData));
		}
		else{
			typedef typename cnVar::TIntegerOfSize<sizeof(TChannel),true>::Type TSNormInt;
			auto SNorm=IntegerVectorMake<ChannelCount>(static_cast<const TSNormInt*>(PixelData));
			SNorm=SNorm.Min(VectorZeroValue);
			return reinterpret_cast<cIntegerVector<TChannel,ChannelCount>&>(SNorm);
		}
	}

	template<class TSourceChannel>
	cIntegerVector<TChannel,ChannelCount> PixelSampler_MatchFloatT(const void *PixelData)noexcept(true){
		auto PixelValue=VectorMake<ChannelCount>(static_cast<const TSourceChannel*>(PixelData));
		PixelValue.Max(VectorFillValue(1.f));
		if cnRTL_IFCONSTEXPR(ChannelIsSigned){
			PixelValue.Min(VectorFillValue(0.f));
		}
		else{
			PixelValue.Min(VectorFillValue(-1.f));
		}
		PixelValue*=VectorFillValue(static_cast<TSourceChannel>(cnVar::TIntegerValue<TChannel>::Max));

		return static_cast< cIntegerVector<TChannel,ChannelCount> >(PixelValue);
	}

	template<class TSourceChannel>
	cIntegerVector<TChannel,ChannelCount> PixelSampler_MatchSIntT(const void *PixelData)noexcept(true){
		auto PixelValue=VectorMake<ChannelCount>(static_cast<const TSourceChannel*>(PixelData));
		if cnRTL_IFCONSTEXPR(ChannelIsSigned){
			PixelValue.Max(VectorFillValue(1.f));
			PixelValue.Min(VectorFillValue(0.f));
			PixelValue*=VectorFillValue(static_cast<TSourceChannel>(cnVar::TIntegerValue<TChannel>::Max));
		}
		else{
		}
		PixelValue*=VectorFillValue(static_cast<TSourceChannel>(cnVar::TIntegerValue<TChannel>::Max));

		return static_cast< cIntegerVector<TChannel,ChannelCount> >(PixelValue);
	}

	template<class TSourceChannel>
	cIntegerVector<TChannel,ChannelCount> PixelSampler_UNormT(const void *PixelData)noexcept(true){
		auto pPixel=static_cast<const TSourceChannel*>(PixelData);

		cIntegerVector<TChannel,ChannelCount> RetPixel;
		for(uIntn i=0;i<fSourceChannelCount;i++){
			RetPixel[i]=pPixel[i];
		}
		return RetPixel;
	}

	template<class TSourceChannel>
	cIntegerVector<TChannel,ChannelCount> PixelSampler_SNormT(const void *PixelData)noexcept(true){
		auto pPixel=static_cast<const TSourceChannel*>(PixelData);

		cIntegerVector<TChannel,ChannelCount> RetPixel;
		for(uIntn i=0;i<fSourceChannelCount;i++){
			RetPixel[i]=pPixel[i];
		}
		return RetPixel;
	}

	template<class TSourceChannel>
	cIntegerVector<TChannel,ChannelCount> PixelSampler_FloatT(const void *PixelData)noexcept(true){
		auto pPixel=static_cast<const TSourceChannel*>(PixelData);

		TSourceChannel NormFactor=static_cast<TSourceChannel>(cnVar::TIntegerValue<TChannel>::Max);
		cIntegerVector<TChannel,ChannelCount> RetPixel;
		for(uIntn i=0;i<fSourceChannelCount;i++){
			RetPixel[i]=pPixel[i]*NormFactor;
		}
		return RetPixel;
	}

	template<class TSourceChannel>
	cIntegerVector<TChannel,ChannelCount> PixelSampler_SIntT(const void *PixelData)noexcept(true){
		auto pPixel=static_cast<const TSourceChannel*>(PixelData);

		cIntegerVector<TChannel,ChannelCount> RetPixel;
		for(uIntn i=0;i<fSourceChannelCount;i++){
			if(pPixel[i]){
				RetPixel[i]=cnVar::TIntegerValue<TChannel>::Max;
			}
			else{
				RetPixel[i]=0;
			}
		}
		return RetPixel;
	}

	template<class TSourceChannel>
	cIntegerVector<TChannel,ChannelCount> PixelSampler_UIntT(const void *PixelData)noexcept(true){
		auto pPixel=static_cast<const TSourceChannel*>(PixelData);

		cIntegerVector<TChannel,ChannelCount> RetPixel;
		for(uIntn i=0;i<fSourceChannelCount;i++){
			if(pPixel[i]){
				RetPixel[i]=cnVar::TIntegerValue<TChannel>::Max;
			}
			else{
				RetPixel[i]=0;
			}
		}
		return RetPixel;
	}

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
class cBitmapSourceRGBASampler1 : public iBitmapSampler<uInt8,4>
{
public:
	cBitmapSourceRGBASampler1()noexcept(true);
	~cBitmapSourceRGBASampler1()noexcept(true);

	virtual void Start(cUIPoint Size)noexcept(true)override;
	virtual void Close(void)noexcept(true)override;
	virtual cVector<uInt8,4> Pixel(void)noexcept(true)override;

	iPtr<iBitmapDataSource> Source;
	cFunction<iPtr<iBitmapDataSource> (cUIPoint Size)> QuerySource;
private:
	iPtr<iBitmapDataSource> fSamplingBitmap;
	const void *fSamplingPixelData;
	cUIPoint fSamplingImageSize;
	uIntn fSamplingPitch;

	class cSampling
	{
	public:
		const void *PixelData;
		cUIPoint ImageSize;
		uIntn Pitch;
		ufInt8 ChannelCount;
		ufInt8 ReadChannelCount;

		virtual ~cSampling()noexcept(true);
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true);
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true);
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true);
	};

	cnVar::cPolymorphicInterface<cSampling> fSampler;

	void SetupSampling8(const cBitmapPixelFormat &PixelFormat)noexcept(true);
	void SetupSampling16(const cBitmapPixelFormat &PixelFormat)noexcept(true);
	void SetupSampling32(const cBitmapPixelFormat &PixelFormat)noexcept(true);

	class cSamplingRGBAUNorm8x4 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingRGBAUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingBGRAUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingAlphaUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingLAUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};


	class cSamplingRGBAUNorm16x4 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingRGBAUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingBGRAUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingAlphaUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingLAUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};


	class cSamplingRGBAFloat32x4 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingRGBAFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingBGRAFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingAlphaFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingLAFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class cBitmapRenderPipeline : public iBitmapSampler<TChannel,ChannelCount>
{
public:
	typedef cIntegerVector<TChannel,ChannelCount> cPixel;
	virtual void Start(cUIPoint Size)noexcept(true)override{
		SourceSampler->Start(Size);
		for(auto &Processor : PixelProcessors){
			Processor->Start(Size);
		}
	}
	virtual void Close(void)noexcept(true)override{
		SourceSampler->Close();
		for(auto &Processor : PixelProcessors){
			Processor->Close();
		}
	}
	virtual cPixel Pixel(void)noexcept(true)override{
		auto Dest=SourceSampler->Pixel();
		for(auto &Processor : PixelProcessors){
			Dest=Processor->Pixel(Dest);
		}
		return Dest;
	}


	rPtr< iBitmapSampler<TChannel,ChannelCount> > SourceSampler;
	cSeqList< rPtr< iBitmapPixelProcessor<TChannel,ChannelCount> > > PixelProcessors;
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount>
class cBitmapPixelProcessor_Noop : public iBitmapPixelProcessor<TChannel,ChannelCount>
{
public:
	virtual void Start(cUIPoint Size)noexcept(true)override{}
	virtual void Close(void)noexcept(true)override{}
	virtual cVector<TChannel,ChannelCount> Pixel(cVector<TChannel,ChannelCount> Dest)noexcept(true)override{
		return Dest;
	}
};
//---------------------------------------------------------------------------
template<class TChannel,uIntn ChannelCount,cVector<TChannel,ChannelCount> PixelProcessor(cVector<TChannel,ChannelCount> Dest,cVector<TChannel,ChannelCount> Src)>
class cBitmapSampleBlendingProcessor : public iBitmapPixelProcessor<TChannel,ChannelCount>
{
public:
	typedef cVector<TChannel,ChannelCount> cPixel;
	virtual void Start(cUIPoint Size)noexcept(true)override{
		SourceSampler->Start(Size);
	}
	virtual void Close(void)noexcept(true)override{
		SourceSampler->Close();
	}
	virtual cPixel Pixel(cPixel Dest)noexcept(true)override{
		auto SrcPixel=SourceSampler->Pixel();
		return PixelProcessor(Dest,SrcPixel);
	}

	rPtr< iBitmapSampler<TChannel,ChannelCount> > SourceSampler;
};
//---------------------------------------------------------------------------
template<class T,uIntn Count>
inline cVector<T,Count> BitmapPixelProcess_Copy(cVector<T,Count>,cVector<T,Count> Src)noexcept(true)
{	return Src;	}

template<class TChannel,uIntn ChannelCount>	using cBitmapSampleBlendingProcessor_Copy
	=cBitmapSampleBlendingProcessor<TChannel,ChannelCount,BitmapPixelProcess_Copy>;

//---------------------------------------------------------------------------

template<class T,uIntn Count>
inline cIntegerVector<T,Count> BitmapPixelProcess_Or(cIntegerVector<T,Count> Dest,cIntegerVector<T,Count> Src)noexcept(true)
{	return Dest | Src;	}

template<class T,uIntn Count>
inline cIntegerVector<T,Count> BitmapPixelProcess_And(cIntegerVector<T,Count> Dest,cIntegerVector<T,Count> Src)noexcept(true)
{	return Dest & Src;	}

template<class T,uIntn Count>
inline cIntegerVector<T,Count> BitmapPixelProcess_Xor(cIntegerVector<T,Count> Dest,cIntegerVector<T,Count> Src)noexcept(true)
{	return Dest ^ Src;	}

template<class TChannel,uIntn ChannelCount>	using cBitmapSampleBlendingProcessor_Or
	=cBitmapSampleBlendingProcessor<TChannel,ChannelCount,BitmapPixelProcess_Or>;
template<class TChannel,uIntn ChannelCount>	using cBitmapSampleBlendingProcessor_And
	=cBitmapSampleBlendingProcessor<TChannel,ChannelCount,BitmapPixelProcess_And>;
template<class TChannel,uIntn ChannelCount>	using cBitmapSampleBlendingProcessor_Xor
	=cBitmapSampleBlendingProcessor<TChannel,ChannelCount,BitmapPixelProcess_Xor>;

//---------------------------------------------------------------------------

template<class T,uIntn Count>
inline cVector<T,Count> BitmapPixelProcess_Add(cVector<T,Count> Dest,cVector<T,Count> Src)noexcept(true)
{	return Dest + Src;	}
template<class T,uIntn Count>
inline cVector<T,Count> BitmapPixelProcess_Mul(cVector<T,Count> Dest,cVector<T,Count> Src)noexcept(true)
{	return Dest * Src;	}

template<class TChannel,uIntn ChannelCount>	using cBitmapSampleBlendingProcessor_Add
	=cBitmapSampleBlendingProcessor<TChannel,ChannelCount,BitmapPixelProcess_Add>;
template<class TChannel,uIntn ChannelCount>	using cBitmapSampleBlendingProcessor_Mul
	=cBitmapSampleBlendingProcessor<TChannel,ChannelCount,BitmapPixelProcess_Mul>;

//---------------------------------------------------------------------------

template<class T,uIntn Count>
inline cVector<T,Count> BitmapPixelProcessAlphaLast_Alpha(cVector<T,Count> Dest,cVector<T,Count> Src)noexcept(true)
{
	T SrcAlpha=Src[Count-1];
	T DestAlpha=Dest[Count-1];

	// dest.rgb = dest*(1-src.a) + src.rgb*src.a
	Dest=Dest*VectorFillValue(1-SrcAlpha);
	Dest+=Src*SrcAlpha;

	// dest.a = dest.a*(1-src.a) + src.a
	DestAlpha=DestAlpha*(1-SrcAlpha) + SrcAlpha;

	auto DestValue=Dest.Unpack();
	DestValue[Count-1]=DestAlpha;
	Dest.Repack();
	return Dest;
}
template<class TChannel,uIntn ChannelCount>	using cBitmapSampleBlendingAlphaLastProcessor_Alpha
	=cBitmapSampleBlendingProcessor<TChannel,ChannelCount,BitmapPixelProcessAlphaLast_Alpha>;

//---------------------------------------------------------------------------
class iBitmapRGBASampler : public iReference
{
public:
	virtual void BeginSample(cUIPoint Size)noexcept(true)=0;
	virtual void EndSample(void)noexcept(true)=0;
	virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)=0;
	virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)=0;
	virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class iBitmapRGBAProcessor : public iReference
{
public:
	virtual cIntegerVector<uInt8,4> BlendUNorm8(cIntegerVector<uInt8,4> DestColorVector,cIntegerVector<uInt8,4> SrcColorVector)noexcept(true)=0;
	virtual cIntegerVector<uInt16,4> BlendUNorm16(cIntegerVector<uInt16,4> DestColorVector,cIntegerVector<uInt8,4> SrcColorVector)noexcept(true)=0;
	virtual cVector<Float32,4> BlendFloat32(cVector<Float32,4> DestColorVector,cVector<uInt8,4> SrcColorVector)noexcept(true)=0;
};
//---------------------------------------------------------------------------
class cBitmapSourceRGBASampler : public iBitmapRGBASampler
{
public:
	cBitmapSourceRGBASampler()noexcept(true);
	~cBitmapSourceRGBASampler()noexcept(true);

	virtual void BeginSample(cUIPoint Size)noexcept(true)override;
	virtual void EndSample(void)noexcept(true)override;
	virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
	virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
	virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;

	iPtr<iBitmapDataSource> Source;
	cFunction<iPtr<iBitmapDataSource> (cUIPoint Size)> QuerySource;
private:
	iPtr<iBitmapDataSource> fSamplingBitmap;
	const void *fSamplingPixelData;
	cUIPoint fSamplingImageSize;
	uIntn fSamplingPitch;

	class cSampling
	{
	public:
		const void *PixelData;
		cUIPoint ImageSize;
		uIntn Pitch;
		ufInt8 ChannelCount;
		ufInt8 ReadChannelCount;

		virtual ~cSampling()noexcept(true);
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true);
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true);
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true);
	};

	cnVar::cPolymorphicInterface<cSampling> fSampler;

	void SetupSampling8(const cBitmapPixelFormat &PixelFormat)noexcept(true);
	void SetupSampling16(const cBitmapPixelFormat &PixelFormat)noexcept(true);
	void SetupSampling32(const cBitmapPixelFormat &PixelFormat)noexcept(true);

	class cSamplingRGBAUNorm8x4 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingRGBAUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingBGRAUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingAlphaUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingLAUNorm8 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};


	class cSamplingRGBAUNorm16x4 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingRGBAUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingBGRAUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingAlphaUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingLAUNorm16 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};


	class cSamplingRGBAFloat32x4 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingRGBAFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingBGRAFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingAlphaFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
	class cSamplingLAFloat32 : public cSampling
	{
	public:
		virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
		virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;
	};
};
//---------------------------------------------------------------------------
class cBitmapRGBAFloatRender : public iBitmapRGBASampler
{
public:
	cBitmapRGBAFloatRender()noexcept(true);
	~cBitmapRGBAFloatRender()noexcept(true);

	virtual void BeginSample(cUIPoint Size)noexcept(true)override;
	virtual void EndSample(void)noexcept(true)override;
	virtual cIntegerVector<uInt8,4> SampleUNorm8(sfInt32 x,sfInt32 y)noexcept(true)override;
	virtual cIntegerVector<uInt16,4> SampleUNorm16(sfInt32 x,sfInt32 y)noexcept(true)override;
	virtual cVector<Float32,4> SampleFloat32(sfInt32 x,sfInt32 y)noexcept(true)override;

	rPtr<iBitmapRGBASampler> SourceSampler;

	class iBlender : public iReference
	{
	public:
		virtual void Begin(cUIPoint Size)noexcept(true)=0;
		virtual void End(void)noexcept(true)=0;
		virtual cVector<Float32,4> Blend(cVector<Float32,4> DestColor,sfInt32 x,sfInt32 y)noexcept(true)=0;
	};
	cSeqList< rPtr<iBlender> > BlendStages;
	cSeqList< cFunction<cVector<Float32,4> (cVector<Float32,4>)> > PixelProcessors;
private:
	cVector<Float32,4> Sample(sfInt32 x,sfInt32 y)noexcept(true);
};
/*
	class cSourceBlender : public cBitmapRGBAFloatRender::iBlender
	{
	public:
		virtual void Begin(cUIPoint Size){
			SourceSampler->BeginSample(Size);
		}
		virtual void End(void){
			SourceSampler->EndSample();
		}
		virtual cVector<Float32,4> Blend(cVector<Float32,4> DestColor,sfInt32 x,sfInt32 y)
		{
			auto SrcColor=SourceSampler->SampleFloat32(x,y);
			return Processor(DestColor,SrcColor);
		}


		rPtr<iBitmapRGBASampler> SourceSampler;
		cnVar::cFunction<cVector<Float32,4> (cVector<Float32,4>,cVector<Float32,4>)> Processor;
	};
	*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

struct cImageBlendingStage
{
	iBitmapDataSource *Source;
	eImageBlendingOperator Operator;
	uInt8 FilterSize;
};
extern const cBitmapPixelFormat BitmapPixel_RGBPA32;
//---------------------------------------------------------------------------
#if	0
class cBitmapPixelProcessor
{
public:
	cBitmapPixelProcessor();
	~cBitmapPixelProcessor();

	bool SetBlendProcess(iBitmapDataSource *Source,eImageBlendingOperator Operator,uInt32 OpColor,uInt32 AddColor);

private:

	iPtr<iBitmapDataSource> fSource;
	uIntn fPitch;
	cUIPoint fSize;
	uInt32 fOpConstant;
	uInt32 fAddConstant;
	eImageBlendingOperator fOperator;
	ufInt8 fPixelSize;

	uInt16 fPixelLockCount=0;
	cMemoryBlock fPixelBuffer;
	void ClearData(void);
	void ProcessSource(void);

	void BlendFunction_Add(uInt32 Color);
	void BlendFunction_Copy(void);
	void BlendFunction_And(void);
	void BlendFunction_Or(void);
	void BlendFunction_Xor(void);
	void BlendFunction_Add(void);
	void BlendFunction_Mul(void);
	void BlendFunction_Alpha(void);

};
#endif
//---------------------------------------------------------------------------
#if	0
class cBitmapRGBAResampleSource : public iBitmapDataSource
{
public:
	cBitmapRGBAResampleSource();
	~cBitmapRGBAResampleSource();

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)override;
	virtual void cnLib_FUNC ReleasePixels(void)override;
	virtual bool cnLib_FUNC IsTopDown(void)override;

	uIntn GetDataSize(void);


	rPtr<iBitmapRGBASampler> SourceSampler;
	cBitmapPixelFormat PixelFormat;
	cUIPoint ImageSize;
	bool TopDown=false;

	void Update(void);

private:
/*
	cUIPoint fSize;
	uInt32 fOpConstant;
	uInt32 fAddConstant;
	eImageBlendingOperator fOperator;
	ufInt8 fPixelSize;
*/
	bool fUpdate;

	uInt16 fPixelLockCount=0;
	cMemoryBlock fPixelBuffer;
	uIntn fPitch;

	void ProcessSource(void);

	void SampleProcessError(void);
	void Sample8Process(void);
	void Sample16Process(void);
	void Sample32Process(void);

	void Sample8ProcessUNormRGBA(void);
	void Sample8ProcessUNormBGRA(void);
	void Sample8ProcessUNormA(void);
	void Sample8ProcessUNormL(void);
	void Sample8ProcessUNormLA(void);

	void Sample32ProcessFloatRGBA(void);
	void Sample32ProcessFloatBGRA(void);
	void Sample32ProcessFloatA(void);
	void Sample32ProcessFloatL(void);
	void Sample32ProcessFloatLA(void);
};
#endif
//---------------------------------------------------------------------------
class cBitmapDataSourceFromBlendingRGBA32 : public iBitmapDataSource
{
public:
	cBitmapDataSourceFromBlendingRGBA32()noexcept(true);
	~cBitmapDataSourceFromBlendingRGBA32()noexcept(true);

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleasePixels(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsTopDown(void)noexcept(true)override;

	bool SetBlending(uIntn StageCount,const cImageBlendingStage *Stages,sfInt32 Width,sfInt32 Height,cBitmapPixelFormat PixelFormat)noexcept(true);

	uIntn GetDataSize(void)noexcept(true);

private:
	cUIPoint fSize;
	cBitmapPixelFormat fPixelFormat;
	bool fTopDown;
	uInt16 fPixelLockCount=0;

	void ClearData(void);
	uIntn fPixelBufferPitch;
	cMemoryBlock fPixelBuffer;

	struct cBlendStage
	{
		iPtr<iBitmapDataSource> Source;
		void (cBitmapDataSourceFromBlendingRGBA32::*BlendFunction)(iBitmapDataSource *Source)noexcept(true);
	};

	cSeqList<cBlendStage> fBlendStages;
	void BlendStages(void)noexcept(true);

	void BlendFunction_Alpha(iBitmapDataSource *Source)noexcept(true);
	void BlendFunction_Copy(iBitmapDataSource *Source)noexcept(true);
	void BlendFunction_Or(iBitmapDataSource *Source)noexcept(true);
	void BlendFunction_And(iBitmapDataSource *Source)noexcept(true);
	void BlendFunction_Xor(iBitmapDataSource *Source)noexcept(true);
	void BlendFunction_Add(iBitmapDataSource *Source)noexcept(true);
	void BlendFunction_Mul(iBitmapDataSource *Source)noexcept(true);
};
iPtr<iBitmapDataSource> CreateBitmapSourceFromBlendingRGBA32(uIntn StageCount,const cImageBlendingStage *Stages,sfInt32 Width=0,sfInt32 Height=0,cBitmapPixelFormat PixelFormat=BitmapPixel_RGBPA32)noexcept(true);
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
/// PixelProcess end

//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace cnRTL{
//---------------------------------------------------------------------------
extern const cUIPoint UIPointZero;
extern const cUIRect UIRectZero;
extern const cUIRectangle UIRectangleZero;
//---------------------------------------------------------------------------
bool UIRectIsEqual(const cUIRect &Rect1,const cUIRect &Rect2)noexcept(true);
//---------------------------------------------------------------------------
bool BitmapColorHasAlpha(eBitmapChannelColor Color)noexcept(true);
//---------------------------------------------------------------------------
struct cBitmapPixelFormatInfo
{
	ufInt8 PixelSize;
	ufInt8 ChannelCount;
	bool HasAlpha;
};
//---------------------------------------------------------------------------
extern const cBitmapPixelFormat BitmapPixelFormatUnknow;
extern const cBitmapPixelFormat BitmapPixelFormatR8G8B8A8;
//---------------------------------------------------------------------------
class cBitmapBuffer : public iBitmapDataSource
{
public:
	cBitmapBuffer()noexcept(true);
	~cBitmapBuffer()noexcept(true);

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleasePixels(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsTopDown(void)noexcept(true)override;

	void* GetDataBuffer(void)const noexcept(true);
	uIntn GetDataSize(void)const noexcept(true);
	bool SetParameter(cBitmapPixelFormat PixelFormat,sfInt32 Width,sfInt32 Height,bool TopDown)noexcept(true);

private:
	uInt16 fBufferLockCount=0;
	bool fTopDown;
	cBitmapPixelFormat fPixelFormat;
	sfInt32 fWidth;
	sfInt32 fHeight;
	uIntn fPixelBufferPitch;
	cMemoryBlock fPixelBuffer;
	void ResetFrameBuffer(void)noexcept(true);
};
//---------------------------------------------------------------------------
class cBitmapDataSourceFromMemory : public iBitmapDataSource
{
public:
	cBitmapDataSourceFromMemory()noexcept(true);
	~cBitmapDataSourceFromMemory()noexcept(true);

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleasePixels(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsTopDown(void)noexcept(true)override;

	bool SetData(cBitmapPixelFormat Format,sfInt32 Width,sfInt32 Height,uIntn PixelDataPitch,const void *PixelData,bool TopDown)noexcept(true);
	const void* GetDataBuffer(void)const noexcept(true);
	uIntn GetDataSize(void)const noexcept(true);

private:
	cBitmapPixelFormat fPixelFormat;
	bool fTopDown;
	uInt16 fBufferLockCount=0;
	sfInt32 fWidth;
	sfInt32 fHeight;
	uIntn fPixelDataPitch;
	const void *fPixelData;
	uIntn fPixelDataSize;
};
iPtr<cBitmapDataSourceFromMemory> CreateBitmapFromMemory(cBitmapPixelFormat PixelFormat,sfInt32 Width,sfInt32 Height,uIntn PixelDataPitchSize,const void *PixelData,bool TopDown)noexcept(true);
//---------------------------------------------------------------------------
class cBitmapResampleBuffer : public iBitmapDataSource
{
public:
	cBitmapResampleBuffer()noexcept(true);
	~cBitmapResampleBuffer()noexcept(true);

	// iBitmapDataSource

	virtual cBitmapPixelFormat cnLib_FUNC GetPixelFormat(void)noexcept(true)override;
	virtual cUIPoint cnLib_FUNC GetImageSize(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC GetDataPitch(void)noexcept(true)override;
	virtual uIntn cnLib_FUNC CopyPixelBuffer(uIntn Offset,void *Dest,uIntn DestSize)noexcept(true)override;
	virtual const void* cnLib_FUNC AcquirePixels(void)noexcept(true)override;
	virtual void cnLib_FUNC ReleasePixels(void)noexcept(true)override;
	virtual bool cnLib_FUNC IsTopDown(void)noexcept(true)override;

	BitmapChannelColor ChannelColor=BitmapChannelColor::RGBA;
	bool AlphaPremultiplied=false;
	bool TopDown=false;

	void SetSampleParameter(sfInt32 Width,sfInt32 Height)noexcept(true){
		fSampleImageWidth=Width;
		fSampleImageHeight=Height;
		fUpdate=true;
	}

	template<class TChannel,uIntn ChannelCount>
	void SetSampler(iBitmapNormSampler<TChannel,ChannelCount> *Sampler)noexcept(true){
		fSampler->~cSampler();
		fSampler.ConstructAs< cSamplerNorm<TChannel,ChannelCount> >();
		auto NormSampler=static_cast<cSamplerNorm<TChannel,ChannelCount>*>(fSampler);
		NormSampler->Sampler=Sampler;
		fUpdate=true;
	}

	template<class TChannel,uIntn ChannelCount>
	void SetSampler(iBitmapSampler<TChannel,ChannelCount> *Sampler)noexcept(true){
		fSampler->~cSampler();
		fSampler.ConstructAs< cSamplerValue<TChannel,ChannelCount> >();
		auto ValueSampler=static_cast<cSamplerValue<TChannel,ChannelCount>*>(fSampler);
		ValueSampler->Sampler=Sampler;
		fUpdate=true;
	}

protected:
	class cSampler
	{
	public:
		virtual ~cSampler()=default;
		virtual void Start(sfInt32 Width,sfInt32 Height)noexcept(true);
		virtual void Close(void)noexcept(true);
		virtual void Pixel(void *PixelBuffer)noexcept(true);
		virtual void SetPixelFormat(cBitmapPixelFormat &Format)noexcept(true);
	};
	class cGenericSampler : public cSampler
	{
	public:
		rPtr<iReference> SourceSampler;
	};

	template<bool IsSigned>
	struct TNormChannelType
	{
		static cnRTL_CONSTVAR BitmapChannelType Value=BitmapChannelType::SNorm;
	};
	template<>
	struct TNormChannelType<false>
	{
		static cnRTL_CONSTVAR BitmapChannelType Value=BitmapChannelType::UNorm;
	};

	template<class TChannel,uIntn ChannelCount>
	class cSamplerNorm : public cSampler
	{
	public:
		rPtr< iBitmapNormSampler<TChannel,ChannelCount> > SourceSampler;
		virtual void Start(sfInt32 Width,sfInt32 Height)noexcept(true)override{
			SourceSampler->Start(Width,Height);
		}
		virtual void Close(void)noexcept(true)override{
			SourceSampler->Close();
		}
		virtual void Pixel(void *PixelBuffer)noexcept(true)override{
			auto Value=SourceSampler->Pixel();
			Value.Store(static_cast<TChannel*>(PixelBuffer));
		}
		virtual void SetPixelFormat(cBitmapPixelFormat &Format)noexcept(true)override{
			Format.ChannelType=TNormChannelType<cnVar::TIsSigned<TChannel>::Value>::Value;
			Format.ChannelSize=sizeof(TChannel);
			Format.ChannelCount=ChannelCount;
		}
	};

	template<bool IsFloat,bool IsSigned>
	struct TValueChannelType
	{
		static cnRTL_CONSTVAR BitmapChannelType Value=BitmapChannelType::Float;
	};
	template<bool IsSigned>
	struct TValueChannelType<false,IsSigned>
	{
		static cnRTL_CONSTVAR BitmapChannelType Value=BitmapChannelType::SIntegral;
	};
	template<>
	struct TValueChannelType<false,false>
	{
		static cnRTL_CONSTVAR BitmapChannelType Value=BitmapChannelType::UIntegral;
	};

	template<class TChannel,uIntn ChannelCount>
	class cSamplerValue : public cSampler
	{
	public:
		rPtr< iBitmapSampler<TChannel,ChannelCount> > SourceSampler;
		virtual void Start(sfInt32 Width,sfInt32 Height)noexcept(true)override{
			SourceSampler->Start(Width,Height);
		}
		virtual void Close(void)noexcept(true)override{
			SourceSampler->Close();
		}
		virtual void Pixel(void *PixelBuffer)noexcept(true)override{
			auto Value=SourceSampler->Pixel();
			Value.Store(static_cast<TChannel*>(PixelBuffer));
		}
		virtual void SetPixelFormat(cBitmapPixelFormat &Format)noexcept(true)override{
			Format.ChannelType=TValueChannelType<
				cnVar::TIsFloat<TChannel>::Value,
				cnVar::TIsSigned<TChannel>::Value
			>::Value;
			Format.ChannelSize=sizeof(TChannel);
			Format.ChannelCount=ChannelCount;
		}
	};
private:
	bool fUpdate;

	sfInt32 fSampleImageWidth;
	sfInt32 fSampleImageHeight;

	ufInt32n fBufferLockCount=0;
	cMemoryBlock fBufferPixelData;
	uIntn fBufferPitch;
	sfInt32 fBufferImageWidth;
	sfInt32 fBufferImageHeight;
	cBitmapPixelFormat fBufferPixelFormat;

	cnVar::cPolymorphicObject<cSampler,cGenericSampler> fSampler;

	void ProcessSource(void)noexcept(true);
};
//---------------------------------------------------------------------------
}	// namespace cnRTL
//---------------------------------------------------------------------------
}	// namespace cnLibrary
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#undef	cnRTL_CONSTEXPR_FUNC
#undef	cnRTL_CONSTVAR
#undef	cnRTL_IFCONSTEXPR
//---------------------------------------------------------------------------
#endif  /* __cplusplus */
/*-------------------------------------------------------------------------*/
#endif

