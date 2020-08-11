namespace iv
{

template< class T >
RandomGenerator::RandomGenerator( Instance * inst, RandomId id, T const & identity ) :
    inst( inst )
{
    // get basic seed
    uint_fast32_t seed = 0;
    if( auto rs = this->instance()->getSystem< RandomSystem >() )
        seed = rs->seed( id );
    
    // combine seed with identity
    this->identity_seed = iv::hash< std::pair< uint_fast32_t, T > >()( std::pair< uint_fast32_t, T >( seed, identity ) );
    
    // seed
    this->reset();
}

}
