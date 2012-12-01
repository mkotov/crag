// Copyright (C) 2006 Alexander Ushakov
// Contents: Part of implementation of class TheGrigorchukGroupAlgorithms
// Algorithms for a normal subgroup generated by b.
//
// Principal Authors: Alexander Ushakov
//
// Revision History:
//


#include "Word.h"
#include "TheGrigorchukGroupAlgorithms.h"


//---------------------------------------------------------------------------//
//----------------------- TheGrigorchukGroupAlgorithms ----------------------//
//---------------------------------------------------------------------------//


pair< Word , list< Word > > TheGrigorchukGroupAlgorithms::decompositionBSbgp( const Word& w )
{
  pair< Word , list< Word > > result;
  Word r = reduce( w );
  
  // state in the Cayley graph of a dihedral group <a,d>
  int state = 0;
  for( Word::const_iterator r_it=r.begin( ) ; r_it!=r.end( ) ; ++r_it ) {
    int g = *r_it;
    
    Word c;
    switch( g ) {
    case 1:
      if( state%2==0 )
	state = (state+1)%8;
      else
	state = (state+7)%8;
      break;
    case 2:
    case 3:
      if( state<=3 )
	for( int i=state-1 ; i>=0 ; --i )
	  c.push_back( i%2==0 ? 1 : 4 );
      else
	for( int i=state ; i<8 ; ++i )
	  c.push_back( i%2==0 ? 1 : 4 );
      result.second.push_back( c );
      if( g==2 )
	break;
    case 4:
      if( state%2==1 )
	state = (state+1)%8;
      else
	state = (state+7)%8;
      break;
    }
  }
  
  for( int i=0 ; i<state ; ++i )
    result.first.push_back( i%2==0 ? 1 : 4 );
  
  return result;
}
