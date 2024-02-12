/*- cnRTL - vector --------------------------------------------------------*/
/*         Developer : Code009                                             */
/*         Create on : 2015-12-03                                          */
/*-------------------------------------------------------------------------*/
#ifndef __cnLibrary_cnTK_Vector_H__
#define __cnLibrary_cnTK_Vector_H__
/*-------------------------------------------------------------------------*/
#include <cnTK/Common.hpp>
#include <cnTK/TypeTraits.hpp>
#include <cnTK/Array.hpp>
/*-------------------------------------------------------------------------*/
#if	cnLibrary_CPPFEATURELEVEL >= 1
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosDeclare.inc>
//---------------------------------------------------------------------------
namespace cnLibrary{
//---------------------------------------------------------------------------
namespace TKRuntime{
//---------------------------------------------------------------------------

template<class TElement>
struct tVectorOperatorEnumeration;
//{
// 	typedef cnVar::TTypePack<TVectorOperator...> tFloatOperatorPack;
// 	typedef cnVar::TTypePack<TVectorOperator...> tIntegerOperatorPack;
//};

//template<class TElement>
//struct TVector;
//{
//};


//---------------------------------------------------------------------------
}	// namespace TKRuntime
//---------------------------------------------------------------------------
namespace cnVar{

//TVectorOperator
//{
//	typedef tVector;
//	typedef tElement;
//
//	static const uIntn ElementCount;
//
//	static tVector Zero(void)noexcept;
//	static tVector Fill(tElement Value)noexcept;
//	static tVector Load(const tElement *Array)noexcept;
//	static void Store(const tVector &Variable,tElement *Array)noexcept;
//	static tVector LoadA(const tElement *Array)noexcept;
//	static void StoreA(const tVector &Variable,tElement *Array)noexcept;
//};

//TFloatVectorOperator : TVectorOperator
//{
//
//	static tVector Add(const tVector &v1,const tVector &v2)noexcept;
//	static tVector Sub(const tVector &v1,const tVector &v2)noexcept;
//	static tVector Mul(const tVector &v1,const tVector &v2)noexcept;
//	static tVector Div(const tVector &v1,const tVector &v2)noexcept;
//};

//TIntegerVectorOperator : TVectorOperator
//{
//	static tVector Add(const tVector &v1,const tVector &v2)noexcept;
//	static tVector Sub(const tVector &v1,const tVector &v2)noexcept;
//	static tVector Mul(const tVector &v1,const tVector &v2)noexcept;
//	static tVector Div(const tVector &v1,const tVector &v2)noexcept;
//
//	static tVector Xor(const tVector &v1,const tVector &v2)noexcept;
//	static tVector And(const tVector &v1,const tVector &v2)noexcept;
//	static tVector Or(const tVector &v1,const tVector &v2)noexcept;
//};

//---------------------------------------------------------------------------

template<class TVectorOperator,uIntn VectorCount>
class cVectorStorage
{
public:
	typedef typename TVectorOperator::tElement tElement;

	operator tElement*()noexcept(true){
		return reinterpret_cast<tElement*>(fVectors);
	}
	operator const tElement*()const noexcept(true){
		return reinterpret_cast<const tElement*>(fVectors);
	}

	tElement operator ()(uIntn Index)noexcept(true){
		return reinterpret_cast<tElement*>(fVectors)[Index];
	}

	void Zero(void)noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			fVectors[i]=TVectorOperator::Zero();
		}
	}

	void Fill(tElement Value)noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			fVectors[i]=TVectorOperator::Fill(Value);
		}
	}


	void Load(const tElement *Array)noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			fVectors[i]=TVectorOperator::Load(Array+i*TVectorOperator::ElementCount);
		}
	}
	void Store(tElement *Array)const noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			TVectorOperator::Store(fVectors[i],Array+i*TVectorOperator::ElementCount);
		}
	}
	void LoadA(const tElement *Array)noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			fVectors[i]=TVectorOperator::LoadA(Array+i*TVectorOperator::ElementCount);
		}
	}
	void StoreA(tElement *Array)const noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			TVectorOperator::StoreA(fVectors[i],Array+i*TVectorOperator::ElementCount);
		}
	}

protected:
	typename TVectorOperator::tVector fVectors[VectorCount];

	template<
		typename TVectorOperator::tVector OpFunc(const typename TVectorOperator::tVector&)
	>
	void UnaryOperator(const cVectorStorage &v)noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			fVectors[i]=OpFunc(v.fVectors[i]);
		}
	}

	template<
		typename TVectorOperator::tVector OpFunc(const typename TVectorOperator::tVector&,const typename TVectorOperator::tVector&)
	>
	void BinaryOperator(const cVectorStorage &v1,const cVectorStorage &v2)noexcept(true){
		for(uIntn i=0;i<VectorCount;i++){
			fVectors[i]=OpFunc(v1.fVectors[i],v2.fVectors[i]);
		}
	}
};

template<class TVectorOperator>
class cVectorStorage<TVectorOperator,0>;	// disable 0 length vector

template<class TVectorOperator>
class cVectorStorage<TVectorOperator,1>
{
public:
	typedef typename TVectorOperator::tElement tElement;
	
	operator tElement*()noexcept(true){
		return reinterpret_cast<tElement*>(&fVector);
	}
	operator const tElement*()const noexcept(true){
		return reinterpret_cast<const tElement*>(&fVector);
	}

	tElement operator ()(uIntn Index)noexcept(true){
		return reinterpret_cast<tElement*>(&fVector)[Index];
	}

	void Zero(void)noexcept(true){
		fVector=TVectorOperator::Zero();
	}

	void Fill(tElement Value)noexcept(true){
		fVector=TVectorOperator::Fill(Value);
	}

	void Load(const tElement *Array)noexcept(true){
		fVector=TVectorOperator::Load(Array);
	}
	void Store(tElement *Array)const noexcept(true){
		TVectorOperator::Store(fVector,Array);
	}
	void LoadA(const tElement *Array)noexcept(true){
		fVector=TVectorOperator::LoadA(Array);
	}
	void StoreA(tElement *Array)const noexcept(true){
		TVectorOperator::StoreA(fVector,Array);
	}

protected:
	typename TVectorOperator::tVector fVector;


	template<
		typename TVectorOperator::tVector OpFunc(const typename TVectorOperator::tVector&)
	>
	void UnaryOperator(const cVectorStorage &v)noexcept(true){
		fVector=OpFunc(v.fVector);
	}

	template<
		typename TVectorOperator::tVector OpFunc(const typename TVectorOperator::tVector&,const typename TVectorOperator::tVector&)
	>
	void BinaryOperator(const cVectorStorage &v1,const cVectorStorage &v2)noexcept(true){
		fVector=OpFunc(v1.fVector,v2.fVector);
	}
};

template<class TOperatorPack,uIntn ElementCount>
struct TVectorOperatorLookup;

template<uIntn ElementCount,class TVectorOperator,class TNextVectorOperator,class...VTVectorOperator>
struct TVectorOperatorLookup<cnVar::TTypePack<TVectorOperator,TNextVectorOperator,VTVectorOperator...>,ElementCount>
	: cnVar::TSelect< (ElementCount<=TVectorOperator::ElementCount)
		, TVectorOperatorLookup<cnVar::TTypePack<TNextVectorOperator,VTVectorOperator...>,ElementCount>
		, cnVar::TTypeDef<TVectorOperator>
	>::Type{};

template<class TMaxVectorOperator,uIntn ElementCount>
struct TVectorOperatorLookup<cnVar::TTypePack<TMaxVectorOperator>,ElementCount>
	: cnVar::TTypeDef<TMaxVectorOperator>{};

template<class TOperatorPack,class TElement,uIntn ElementCount>
struct TVectorOperatorSelector
{
	typedef typename TVectorOperatorLookup<
		TOperatorPack,
		ElementCount
	>::Type tVectorOperator;

	typedef cVectorStorage<tVectorOperator,
		(ElementCount+tVectorOperator::ElementCount-1)/tVectorOperator::ElementCount
	> tVectorStorage;
};


template<class TElement,uIntn ElementCount>
class cVector : public TVectorOperatorSelector<typename TKRuntime::tVectorOperatorEnumeration<TElement>::tFloatOperatorPack,TElement,ElementCount>::tVectorStorage
{
public:
	typedef TElement tElement;
	typedef typename TVectorOperatorSelector<typename TKRuntime::tVectorOperatorEnumeration<TElement>::tFloatOperatorPack,TElement,ElementCount>::tVectorStorage tVectorStorage;
	typedef typename TVectorOperatorSelector<typename TKRuntime::tVectorOperatorEnumeration<TElement>::tFloatOperatorPack,TElement,ElementCount>::tVectorOperator tVectorOperator;

	using tVectorStorage::operator =;

	static cnLib_CONSTVAR uIntn PaddingLength=ElementCount%tVectorOperator::ElementCount;

	cVector& operator += (const cVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Add>(*this,src);
		return *this;
	}
	cVector& operator -= (const cVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Sub>(*this,src);
		return *this;
	}
	cVector& operator *= (const cVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Mul>(*this,src);
		return *this;
	}
	cVector& operator /= (const cVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Div>(*this,src);
		return *this;
	}

	cVector operator + (const cVector &src)const noexcept(true){
		cVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Add>(*this,src);
		return RetValue;
	}
	cVector operator - (const cVector &src)const noexcept(true){
		cVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Sub>(*this,src);
		return RetValue;
	}
	cVector operator * (const cVector &src)const noexcept(true){
		cVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Mul>(*this,src);
		return RetValue;
	}
	cVector operator / (const cVector &src)const noexcept(true){
		cVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Div>(*this,src);
		return RetValue;
	}

	template<uIntn MulCount,class TLeftColumnAccessor>
	void MatrixMultiplyL(TLeftColumnAccessor LeftColumns,const cVector *RightRows)noexcept(true){
	/*
		L0*R00+L1*R10+L2*R20	,	L0*R01+L1*R11+L2*R21	,	L0*R02+L1*R12+L2*R22	, ...

		result row:
		<R00,R01,R02> * L0 =  <Lr0*R00, L0*R01, L0*R02>
						+
		<R10,R11,R12> * L1 =  <Lr1*R10, L1*R11, L1*R12>
						+
		<R20,R21,R22> * L2 =  <Lr2*R20, L2*R21, L2*R22>
						=<L0*R00+L1*R10+L2*R20	,	L0*R01+L1*R11+L2*R21	,	L0*R02+L1*R12+L2*R22>
	*/
		// result vector
		this->Zero();

		for(uIntn MulIndex=0;MulIndex<MulCount;MulIndex++){
			cVector TempValue;
			cVector FillValue;
			FillValue.Fill(LeftColumns(MulIndex));
			TempValue.template BinaryOperator<tVectorOperator::Mul>(
				RightRows[MulIndex],
				FillValue
			);
			this->template BinaryOperator<tVectorOperator::Add>(*this,TempValue);
		}
	}

	template<uIntn MulCount,class TRightRowAccessor>
	void MatrixMultiplyR(const cVector *LeftColumns,TRightRowAccessor RightRows)noexcept(true){
	/*
		L00*R0+L01*R1+L02*R2
		L10*R0+L11*R1+L12*R2
		L20*R0+L21*R1+L22*R2
		...

		result col:
		<L00,L10,L20> * R0c =  <L00*R0, L10*R0, L20*R0>
						+
		<L01,L11,L21> * R1c =  <L01*R1, L11*R1, L21*R1>
						+
		<L02,L12,L22> * R2c =  <L02*R2, L12*R2, L22*R2>
						=<L00*R0+L01*R1+L02*R2	,	L10*R0+L11*R1+L12*R2	,	L20*R0+L21*R1+L22*R2>
	*/
		// result rows
		this->Zero();

		for(uIntn MulIndex=0;MulIndex<MulCount;MulIndex++){
			cVector TempValue;
			cVector FillValue;
			FillValue.Fill(RightRows(MulIndex));
			TempValue.template BinaryOperator<tVectorOperator::Mul>(
				LeftColumns[MulIndex],
				FillValue
			);
			this->template BinaryOperator<tVectorOperator::Add>(*this,TempValue);
		}
	}
};

//---------------------------------------------------------------------------
template<class TElement,uIntn RowCount,uIntn ColCount>	class cMatrixR;
template<class TElement,uIntn RowCount,uIntn ColCount>	class cMatrixC;
//---------------------------------------------------------------------------

template<class TElement,uIntn RowCount,uIntn ColCount>
class cMatrixR
{
public:
	typedef cVector<TElement,ColCount> tVector;
	typedef typename tVector::tElement tElement;
	
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cMatrixR()=default;
#else
	cnLib_CONSTEXPR_FUNC cMatrixR(){}
#endif // !cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT


#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L
	template<class...TArgs>
	cMatrixR(tElement v0,TArgs...Args)noexcept(true)
	{
		tElement Array[RowCount*ColCount]={v0,static_cast<tElement>(Args)...};
		Load(Array);
	}
#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	cMatrixR(const cMatrixR &Src)noexcept(true){
		for(uIntn i=0;i<RowCount;i++){
			fRow[i]=Src.fRow[i];
		}
	}
	cMatrixR& operator =(const cMatrixR &Src)noexcept(true){
		if(this!=&Src){
			for(uIntn i=0;i<RowCount;i++){
				fRow[i]=Src.fRow[i];
			}
		}
		return *this;
	}

	cMatrixR(const cMatrixC<TElement,RowCount,ColCount> &Src)noexcept(true){
		alignas(tVector) tElement Elements[RowCount*ColCount];
		Src.StoreA(Elements);
		LoadTranspose(Elements);
	}
	cMatrixR& operator =(const cMatrixC<TElement,RowCount,ColCount> &Src)noexcept(true){
		alignas(tVector) tElement Elements[RowCount*ColCount];
		Src.StoreA(Elements);
		LoadTranspose(Elements);
	}

	void Load(const tElement *Array)noexcept(true){
		for(uIntn i=0;i<RowCount;i++){
			fRow[i].Load(Array+i*ColCount);
		}
	}
	void Store(tElement *Array)const noexcept(true){
		for(uIntn i=0;i<RowCount;i++){
			fRow[i].Store(Array+i*ColCount);
		}
	}
	void LoadA(const tElement *Array)noexcept(true){
		for(uIntn i=0;i<RowCount;i++){
			fRow[i].LoadA(Array+i*ColCount);
		}
	}
	void StoreA(tElement *Array)const noexcept(true){
		for(uIntn i=0;i<RowCount;i++){
			fRow[i].StoreA(Array+i*ColCount);
		}
	}
	void LoadTranspose(const tElement *Array)noexcept(true){
		for(uIntn row=0;row<RowCount;row++){
			alignas(tVector) tElement TempArray[RowCount];
			for(uIntn col=0;col<ColCount;col++){
				TempArray[col]=Array[col*RowCount+row];
			}
			fRow[row].LoadA(TempArray);
		}
	}

	operator tVector* (void)noexcept(true)			{	return fRow;	}
	operator const tVector* (void)const	noexcept(true){	return fRow;	}

	tElement operator ()(uIntn Row,uIntn Col)noexcept(true){
		return fRow[Row][Col];
	}

	typename TTypeConditional<tElement*,tVector::PaddingLength==0>::Type Cells(void)noexcept(true)
	{	return static_cast<tElement*>(fRow[0]);	}
	typename TTypeConditional<const tElement*,tVector::PaddingLength==0>::Type Cells(void)const noexcept(true)
	{	return static_cast<const tElement*>(fRow[0]);	}

	const cMatrixC<TElement,ColCount,RowCount>& Transpose(void)const noexcept(true){
		return *reinterpret_cast<const cMatrixC<TElement,ColCount,RowCount>*>(this);
	}

	template<uIntn MulCount,class TLeftCellAccessor>
	void MatrixMultiply(TLeftCellAccessor LeftCells,const tVector *RightRows)noexcept(true){
	/*
		L00*R00+L01*R10+L02*R20	,	L00*R01+L01*R11+L02*R21	,	L00*R02+L01*R12+L02*R22	, ...
		L10*R00+L11*R10+L12*R20	,	L10*R01+L11*R11+L12*R21	,	L10*R02+L11*R12+L12*R22
		L20*R00+L21*R10+L22*R20	,	L20*R01+L21*R11+L22*R21	,	L20*R02+L21*R12+L22*R22
		...

		row r:
		<R00,R01,R02> * Lr0 =  <Lr0*R00, Lr0*R01, Lr0*R02>
						+
		<R10,R11,R12> * Lr1 =  <Lr1*R10, Lr1*R11, Lr1*R12>
						+
		<R20,R21,R22> * Lr2 =  <Lr2*R20, Lr2*R21, Lr2*R22>
						=<Lr0*R00+Lr1*R10+Lr2*R20	,	Lr0*R01+Lr1*R11+Lr2*R21	,	Lr0*R02+Lr1*R12+Lr2*R22>
	*/
		// result rows
		for(uIntn Row=0;Row<RowCount;Row++){
			fRow[Row].Zero();
		}

		for(uIntn MulIndex=0;MulIndex<MulCount;MulIndex++){
			for(uIntn Row=0;Row<RowCount;Row++){
				tVector t=RightRows[MulIndex] * VectorFill(LeftCells(Row,MulIndex));
				fRow[Row]+=t;
			}
		}

	}
protected:
	tVector fRow[RowCount];
};

template<class TElement,uIntn RowCount,uIntn ColCount>
class cMatrixC
{
public:
	typedef cVector<TElement,RowCount> tVector;
	typedef typename tVector::tElement tElement;
	
#ifndef cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT
	cMatrixC()=default;
#else
	cnLib_CONSTEXPR_FUNC cMatrixC(){}
#endif // !cnLibrary_CPPEXCLUDE_CLASS_MEMBER_DEFAULT

#if cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	template<class...TArgs>
	cMatrixC(tElement v0,TArgs...Args)noexcept(true)
	{
		alignas(tVector) tElement Array[RowCount*ColCount]={v0,static_cast<tElement>(Args)...};
		LoadA(Array);
	}

#endif // cnLibrary_CPPFEATURE_VARIADIC_TEMPLATES >= 200704L

	cMatrixC(const cMatrixC &Src)noexcept(true){
		for(uIntn i=0;i<ColCount;i++){
			fCol[i]=Src.fCol[i];
		}
	}
	cMatrixC& operator =(const cMatrixC &Src)noexcept(true){
		if(this!=&Src){
			for(uIntn i=0;i<ColCount;i++){
				fCol[i]=Src.fCol[i];
			}
		}
		return *this;
	}

	cMatrixC(const cMatrixR<TElement,RowCount,ColCount> &Src)noexcept(true){
		alignas(tVector) tElement Elements[RowCount*ColCount];
		Src.StoreA(Elements);
		LoadTranspose(Elements);
	}
	cMatrixC& operator =(const cMatrixR<TElement,RowCount,ColCount> &Src)noexcept(true){
		alignas(tVector) tElement Elements[RowCount*ColCount];
		Src.StoreA(Elements);
		LoadTranspose(Elements);
	}


	void Load(const tElement *Array)noexcept(true){
		for(uIntn i=0;i<ColCount;i++){
			fCol[i].Load(Array+i*RowCount);
		}
	}
	void Store(tElement *Array)const noexcept(true){
		for(uIntn i=0;i<ColCount;i++){
			fCol[i].Store(Array+i*RowCount);
		}
	}
	void LoadA(const tElement *Array)noexcept(true){
		for(uIntn i=0;i<ColCount;i++){
			fCol[i].LoadA(Array+i*RowCount);
		}
	}
	void StoreA(tElement *Array)const noexcept(true){
		for(uIntn i=0;i<ColCount;i++){
			fCol[i].StoreA(Array+i*RowCount);
		}
	}
	void LoadTranspose(const tElement *Array)noexcept(true){
		for(uIntn col=0;col<ColCount;col++){
			alignas(tVector) tElement TempArray[RowCount];
			for(uIntn row=0;row<RowCount;row++){
				TempArray[row]=Array[row*RowCount+col];
			}
			fCol[col].LoadA(TempArray);
		}
	}


	const cMatrixR<TElement,ColCount,RowCount>& Transpose(void)const noexcept(true){
		return *reinterpret_cast<const cMatrixR<TElement,ColCount,RowCount>*>(this);
	}

	operator tVector* (void)noexcept(true)			{	return fCol;	}
	operator const tVector* (void)const	noexcept(true){	return fCol;	}

	tElement operator ()(uIntn Row,uIntn Col)noexcept(true){
		return fCol[Col][Row];
	}

	typename TTypeConditional<tElement*,tVector::PaddingLength==0>::Type Cells(void)noexcept(true)
	{	return static_cast<tElement*>(fCol[0]);	}
	typename TTypeConditional<const tElement*,tVector::PaddingLength==0>::Type Cells(void)const noexcept(true)
	{	return static_cast<const tElement*>(fCol[0]);	}

	template<uIntn MulCount,class TRightCellAccessor>
	void MatrixMultiply(const tVector *LeftColumns,TRightCellAccessor RightCells)noexcept(true){
	/*
		L00*R00+L01*R10+L02*R20	,	L00*R01+L01*R11+L02*R21	,	L00*R02+L01*R12+L02*R22	, ...
		L10*R00+L11*R10+L12*R20	,	L10*R01+L11*R11+L12*R21	,	L10*R02+L11*R12+L12*R22
		L20*R00+L21*R10+L22*R20	,	L20*R01+L21*R11+L22*R21	,	L20*R02+L21*R12+L22*R22
		...

		col c:
		<L00,L10,L20> * R0c =  <L00*R0c, L10*R0c, L20*R0c>
						+
		<L01,L11,L21> * R1c =  <L01*R1c, L11*R1c, L21*R1c>
						+
		<L02,L12,L22> * R2c =  <L02*R2c, L12*R2c, L22*R2c>
						=<L00*R0c+L01*R1c+L02*R2c	,	L10*R0c+L11*R1c+L12*R2c	,	L20*R0c+L21*R1c+L22*R2c>
	*/
		// result columns
		for(uIntn Col=0;Col<ColCount;Col++){
			fCol[Col].Zero();
		}

		for(uIntn MulIndex=0;MulIndex<MulCount;MulIndex++){
			for(uIntn Col=0;Col<ColCount;Col++){
				tVector RightRow;
				RightRow.Fill(RightCells(MulIndex,Col));
				tVector t=LeftColumns[MulIndex] * RightRow;
				fCol[Col]+=t;
			}
		}

	}
protected:
	tVector fCol[ColCount];
};


// operator cMatrixR = cMatrixR * cMatrixR

template<class TElement,uIntn LRowCount,uIntn MulCount, uIntn RColCount>
inline cMatrixR<TElement,LRowCount,RColCount> operator * (
	const cMatrixR<TElement,LRowCount,MulCount> &Left,
	const cMatrixR<TElement,MulCount,RColCount> &Right
)noexcept(true){
	cMatrixR<TElement,LRowCount,RColCount> mr;
	mr.template MatrixMultiply<MulCount>(Left,Right);
	return mr;
}

// operator cMatrixR = cMatrixC * cMatrixR

template<class TElement,uIntn LRowCount,uIntn MulCount, uIntn RColCount>
inline cMatrixR<TElement,LRowCount,RColCount> operator * (
	const cMatrixC<TElement,LRowCount,MulCount> &Left,
	const cMatrixR<TElement,MulCount,RColCount> &Right
)noexcept(true){
	cMatrixR<TElement,LRowCount,RColCount> mr;
	mr.template MatrixMultiply<MulCount>(Left,Right);
	return mr;
}

// operator cMatrixC = cMatrixC * cMatrixC

template<class TElement,uIntn LRowCount,uIntn MulCount, uIntn RColCount>
inline cMatrixC<TElement,LRowCount,RColCount> operator * (
	const cMatrixC<TElement,LRowCount,MulCount> &Left,
	const cMatrixC<TElement,MulCount,RColCount> &Right
)noexcept(true){
	cMatrixC<TElement,LRowCount,RColCount> mr;
	mr.template MatrixMultiply<MulCount>(Left,Right);
	return mr;
}

// operator cMatrixC = cMatrixC * cMatrixR

template<class TElement,uIntn LRowCount,uIntn MulCount, uIntn RColCount>
inline cMatrixC<TElement,LRowCount,RColCount> operator * (
	const cMatrixC<TElement,LRowCount,MulCount> &Left,
	const cMatrixR<TElement,MulCount,RColCount> &Right
)noexcept(true){
	cMatrixC<TElement,LRowCount,RColCount> mr;
	mr.template MatrixMultiply<MulCount>(Left,Right);
	return mr;
}


//---------------------------------------------------------------------------

// operator cVector = cVector * cMatrixR
template<class TElement,uIntn RowCount,uIntn ColCount>
inline cVector<TElement,ColCount> operator * (
	const cVector<TElement,ColCount> &v,
	const cMatrixR<TElement,RowCount,ColCount> &M
)noexcept(true){
	cVector<ColCount> Result;
	Result.template MatrixMultiplyL<RowCount>(v,M);
	return Result;
}

//---------------------------------------------------------------------------

// operator cVector = cMatrixC * cVector
template<class TElement,uIntn RowCount,uIntn ColCount>
inline cVector<TElement,RowCount> operator * (
	const cMatrixC<TElement,RowCount,ColCount> &M,
	const cVector<TElement,RowCount> &v
)noexcept(true){
	cVector<TElement,RowCount> Result;
	Result.template MatrixMultiplyR<RowCount>(M,v);
	return Result;
}


//---------------------------------------------------------------------------
template<class TElement,uIntn ElementCount>
class cIntegerVector : public TVectorOperatorSelector<typename TKRuntime::tVectorOperatorEnumeration<TElement>::tIntegerOperatorPack,TElement,ElementCount>::tVectorStorage
{
public:
	typedef TElement tElement;
	typedef typename TVectorOperatorSelector<typename TKRuntime::tVectorOperatorEnumeration<TElement>::tIntegerOperatorPack,TElement,ElementCount>::tVectorOperator tVectorOperator;

	static cnLib_CONSTVAR uIntn PaddingLength=ElementCount%tVectorOperator::ElementCount;

	cIntegerVector& operator += (const cIntegerVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Add>(*this,src);
		return *this;
	}
	cIntegerVector& operator -= (const cIntegerVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Sub>(*this,src);
		return *this;
	}
	cIntegerVector& operator *= (const cIntegerVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Mul>(*this,src);
		return *this;
	}
	cIntegerVector& operator /= (const cIntegerVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Div>(*this,src);
		return *this;
	}

	cIntegerVector operator + (const cIntegerVector &src)const noexcept(true){
		cIntegerVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Add>(*this,src);
		return RetValue;
	}
	cIntegerVector operator - (const cIntegerVector &src)const noexcept(true){
		cIntegerVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Sub>(*this,src);
		return RetValue;
	}
	cIntegerVector operator * (const cIntegerVector &src)const noexcept(true){
		cIntegerVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Mul>(*this,src);
		return RetValue;
	}
	cIntegerVector operator / (const cIntegerVector &src)const noexcept(true){
		cIntegerVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Div>(*this,src);
		return RetValue;
	}
	

	cIntegerVector& operator ^= (const cIntegerVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Xor>(*this,src);
		return *this;
	}
	cIntegerVector operator ^ (const cIntegerVector &src)const noexcept(true){
		cIntegerVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Xor>(*this,src);
		return RetValue;
	}

	cIntegerVector& operator &= (const cIntegerVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::And>(*this,src);
		return *this;
	}
	cIntegerVector operator & (const cIntegerVector &src)const noexcept(true){
		cIntegerVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::And>(*this,src);
		return RetValue;
	}

	cIntegerVector& operator |= (const cIntegerVector &src)noexcept(true){
		this->template BinaryOperator<tVectorOperator::Or>(*this,src);
		return *this;
	}
	cIntegerVector operator | (const cIntegerVector &src)const noexcept(true){
		cIntegerVector RetValue;
		RetValue.template BinaryOperator<tVectorOperator::Or>(*this,src);
		return RetValue;
	}
};
//---------------------------------------------------------------------------
template<class T>
struct cScalerVectorOperator
{
	typedef T tElement;
	typedef T tVector;
	static cnLib_CONSTVAR uIntn ElementCount=1;

	static T Zero(void)noexcept(true){
		return 0;
	}
	static T Fill(T Value)noexcept(true){
		return Value;
	}
	static T Load(const T *Array)noexcept(true){
		return Array[0];
	}
	static void Store(const T &Variable,T *Array)noexcept(true){
		Array[0]=Variable;
	}
	static T LoadA(const T *Array)noexcept(true){
		return Array[0];
	}
	static void StoreA(const T &Variable,T *Array)noexcept(true){
		Array[0]=Variable;
	}

	static T Add(const T &v1,const T &v2)noexcept(true){
		return v1+v2;
	}
	static T Sub(const T &v1,const T &v2)noexcept(true){
		return v1-v2;
	}

	static T Mul(const T &v1,const T &v2)noexcept(true){
		return v1*v2;
	}
	static T Div(const T &v1,const T &v2)noexcept(true){
		return v1/v2;
	}
};
//---------------------------------------------------------------------------
template<class T>
struct cScalerFloatVectorOperator : cScalerVectorOperator<T>
{
};
//---------------------------------------------------------------------------
template<class T>
struct cScalerIntegerVectorOperator : public cScalerVectorOperator<T>
{
	static T Xor(const T &v1,const T &v2)noexcept(true){
		return v1^v2;
	}
	static T And(const T &v1,const T &v2)noexcept(true){
		return v1&v2;
	}
	static T Or(const T &v1,const T &v2)noexcept(true){
		return v1|v2;
	}
};
//---------------------------------------------------------------------------
}   // namespace cnVar
//---------------------------------------------------------------------------
}   // namespace cnLibrary
//---------------------------------------------------------------------------
#include <cnTK/TKMacrosCleanup.inc>
//---------------------------------------------------------------------------
#endif  /* cnLibrary_CPPFEATURELEVEL >= 1 */
/*-------------------------------------------------------------------------*/
#endif
