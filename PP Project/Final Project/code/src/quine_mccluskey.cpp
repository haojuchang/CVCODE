#include "quine_mccluskey.h"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>
#include <sstream>
#include <cstdio>
#include <chrono>
#include <omp.h>

// #define VER1
#define VER2

QuineMcCluskey::Implicant::Implicant( const std::string& input_string )
    : booleans()
{
    booleans.resize( input_string.size() );
    for ( int i = 0; i < static_cast<int>( input_string.size() ); ++i )
    {
        if ( input_string[i] == '0' )
        {
            booleans[i] = ZERO;
        }
        else if ( input_string[i] == '1' )
        {
            booleans[i] = ONE;
        }
        else if ( input_string[i] == '-' )
        {
            booleans[i] = DONT_CARE;
        }
        else
        {
            std::cerr << "Invalid input" << std::endl;
            assert( false );
        }
     }
}

int QuineMcCluskey::Implicant::get_ones_count() const
{
    int ones_count = 0;
    for ( int i = 0; i < static_cast<int>( booleans.size() ); ++i )
    {
        if ( booleans[i] == ONE )
        {
            ++ones_count;
        }
    }
    return ones_count;
}

int QuineMcCluskey::Implicant::get_dont_cares_count() const
{
    int dont_cares_count = 0;
    for ( int i = 0; i < static_cast<int>( booleans.size() ); ++i )
    {
        if ( booleans[i] == DONT_CARE )
        {
            ++dont_cares_count;
        }
    }
    return dont_cares_count;
}

int QuineMcCluskey::Implicant::get_cost() const
{
    int cost = 0;
    for ( int i = 0; i < static_cast<int>( booleans.size() ); ++i )
    {
        if ( booleans[i] != DONT_CARE )
        {
            ++cost;
        }
    }
    return cost;
}

void QuineMcCluskey::Implicant::set_size( int size )
{
    booleans.resize( size );
}

void QuineMcCluskey::Implicant::set_value( int index, Boolean value )
{
    if ( index < 0 || index >= static_cast<int>( booleans.size() ) )
    {
        std::cerr << "Out of the bound of minterm's range while settting its value" << std::endl;
        assert( false );
    }
    booleans[index] = value;
}

std::ostream& operator<<( std::ostream& os, const QuineMcCluskey::Implicant& min_term )
{
    for ( int i = 0; i < static_cast<int>( min_term.booleans.size() ); ++i )
    {
        if ( min_term.booleans[i] == QuineMcCluskey::Implicant::ONE )
        {
            os << "1";
        }
        else if ( min_term.booleans[i] == QuineMcCluskey::Implicant::ZERO )
        {
            os << "0";
        }
        else
        {
            os << "-";
        }
    }
    return os;
}

void QuineMcCluskey::Implicant::get_on_set_values( std::vector<int>& on_set_values ) const
{
    if ( on_set_values.size() != 0 )
    {
        std::cerr << "Warn: Putting non empty vector as the function argument to output the on set values of some implicants";
    }
    on_set_values.resize( 0 );
    std::vector<int> dont_care_ids;
    int offset = 0;
    for ( int i = 0; i < static_cast<int>( booleans.size() ); ++i )
    {
        if ( booleans[i] == ONE )
        {
            offset += std::pow( 2, i );
        }
        else if ( booleans[i] == DONT_CARE )
        {
            dont_care_ids.push_back( i );
        }
    }
    
    on_set_values.push_back( offset );
    for( int i = 0; i < static_cast<int>( dont_care_ids.size() ); ++i )
    {
        std::vector<int> new_values;
        for ( int j = 0; j < static_cast<int>( on_set_values.size() ); ++j )
        {
            new_values.push_back( on_set_values[j] + std::pow( 2, dont_care_ids[i] ) );   
        }
        on_set_values.insert( on_set_values.end(), new_values.begin(), new_values.end() );
    }
}

bool operator<( const QuineMcCluskey::Implicant& lhs, const QuineMcCluskey::Implicant& rhs )
{
    if ( lhs.booleans.size() != rhs.booleans.size() )
    {
        std::cerr << "Inconsistent input implicants' size found while comparing them" << std::endl;
        assert( false );
    }

    // comparing dont cares' count first
    // then ones count
    // then the content inside
    if ( lhs.get_dont_cares_count() < rhs.get_dont_cares_count() )
    {
        return true;
    }
    else
    {
        if ( lhs.get_ones_count() < rhs.get_ones_count() )
        {
            return true;
        }
        else
        {
            for ( int i = 0; i < static_cast<int>( lhs.booleans.size() ); ++i )
            {
                if ( lhs.booleans[i] != rhs.booleans[i] )
                {
                    if ( lhs.booleans[i] == QuineMcCluskey::Implicant::ZERO )
                    {
                        return true;
                    }
                    else if ( lhs.booleans[i] == QuineMcCluskey::Implicant::ONE )
                    {
                        return ( rhs.booleans[i] == QuineMcCluskey::Implicant::DONT_CARE );
                    }
                    else
                    {
                        return false;
                    }
                }
            }
            return false;
        }
    }
}

bool operator==( const QuineMcCluskey::Implicant& lhs, const QuineMcCluskey::Implicant& rhs )
{
    if ( lhs.booleans.size() != rhs.booleans.size() )
    {
        std::cerr << "Inconsistent min terms' size found while comparing them" << std::endl;
        assert( false );
    }

    for ( int i = 0; i < static_cast<int>( lhs.booleans.size() ); ++i )
    {
        if ( lhs.booleans[i] != rhs.booleans[i] )
        {
            return false;
        }
    }
    return true;
}

void QuineMcCluskey::_check_variable_count_consistent( const std::vector<std::string>& minterms ) const
{
    int variable_count = static_cast<int>( minterms[0].size() );
    for ( int i = 0; i < static_cast<int>( minterms.size() ); ++i )
    {
        int current_variable_count = static_cast<int>( minterms[i].size() );
        if ( current_variable_count != variable_count )
        {
            std::cerr << "Error: Found inconsistent number of variables in input minterms" << std::endl;
            assert( false );
        }
    }
}

QuineMcCluskey::QuineMcCluskey()
    : _qm_table_ptr(), _input_implicant_set(), _prime_implicant_set()
{
}

bool QuineMcCluskey::_initialize_qm_table( const std::vector<std::string>& input_implicants )
{
    _qm_table_ptr.reset( new QMTable );
    _input_implicant_set.resize( 0 );
    if ( input_implicants.size() == 0 )
    {
        std::cerr << "Warn: Empty input implicants while calling Quine McCluskey" << std::endl;
        return false;
    }
    _check_variable_count_consistent( input_implicants );

    int variable_count = static_cast<int>( input_implicants[0].size() );
    _qm_table_ptr->resize( variable_count + 1 );

    #pragma omp parallel for
    for ( int i = 0; i < static_cast<int>( input_implicants.size() ); ++i )
    {
        Implicant implicant( input_implicants[i] );
        int ones_count = implicant.get_ones_count();
        #pragma omp critical
        {
            ( *_qm_table_ptr )[ones_count].push_back( implicant );
            _input_implicant_set.push_back( implicant );
        }
    }
    return true;
}

void QuineMcCluskey::run( const std::vector<std::string>& input_implicants )
{
    bool is_input_valid = _initialize_qm_table( input_implicants );
    if ( !is_input_valid )
    {
        return;
    }

    std::cout << std::endl;
    bool is_at_least_one_pair_merged;
    int iteration = 1;
    do
    {
        std::cout << "QM iteration : " << iteration << std::endl;
        _remove_redundant_implicants();
        is_at_least_one_pair_merged = _try_merge_all_pairs_of_implicants();
        ++iteration;
    } while ( is_at_least_one_pair_merged );
    _remove_redundant_implicants();
    _collect_prime_implicants_from_qm_table();
}

bool QuineMcCluskey::_try_merge_all_pairs_of_implicants()
{
    _report_qm_table();
    auto& _qm_table = *_qm_table_ptr;
    // initialize variables for next iteration
    std::unique_ptr<QMTable> new_qm_table_ptr( new QMTable );
    new_qm_table_ptr->resize( _qm_table.size() - 1 );
    std::vector<std::vector<bool> > is_implicant_merged( _qm_table.size() );
    for ( int group_id = 0; group_id < static_cast<int>( _qm_table.size() ); ++group_id )
    {
        is_implicant_merged[group_id].resize( _qm_table[group_id].size(), false );
    }

    auto& new_qm_table = *new_qm_table_ptr;

#if defined( VER1 )

    int group_pairs_count = static_cast<int>( _qm_table.size() ) - 1;
    bool is_at_least_one_pair_merged = false;
    for ( int i = 0; i < group_pairs_count; ++i )
    {
        const auto& group1 = _qm_table[i];
        const auto& group2 = _qm_table[i + 1];
        #pragma omp parallel for collapse( 2 ) reduction( | : is_at_least_one_pair_merged )
        for ( int j = 0; j < static_cast<int>( group1.size() ); ++j )
        {
            for ( int k = 0; k < static_cast<int>( group2.size() ); ++k )
            {
                bool is_merged = _try_merge_two_implicants( group1[j], group2[k], new_qm_table[i] );
                is_at_least_one_pair_merged |= is_merged;
                if ( is_merged )
                {
                    #pragma omp critical
                    {
                        is_implicant_merged[i][j] = true;
                        is_implicant_merged[i + 1][k] = true;
                    }
                }
            }
        }
    }
    #pragma omp parallel for
    for ( int i = 0; i < static_cast<int>( is_implicant_merged.size() ); ++i )
    {
        for ( int j = 0; j < static_cast<int>( is_implicant_merged[i].size() ); ++j )
        {
            if ( !is_implicant_merged[i][j] )
            {   
                #pragma omp critical
                {
                    _prime_implicant_set.push_back( _qm_table[i][j] );
                }
            }       
        }
    }

#elif defined ( VER2 )
    
    int group_pairs_count = static_cast<int>( _qm_table.size() ) - 1;
    bool is_at_least_one_pair_merged = false;
    for ( int i = 0; i < group_pairs_count; ++i )
    {
        const auto& group1 = _qm_table[i];
        const auto& group2 = _qm_table[i + 1];
        #pragma omp parallel
        {
            QMTable thread_private_qm_table( _qm_table.size() - 1 );
            std::vector<std::vector<bool>> thread_private_is_implicant_merged( _qm_table.size() );
            for ( int i = 0; i < static_cast<int>( _qm_table.size() ); ++i )
            {
                thread_private_is_implicant_merged[i].resize( _qm_table[i].size(), false );
            }

            #pragma omp for collapse( 2 ) reduction( | : is_at_least_one_pair_merged ) nowait
            for ( int j = 0; j < static_cast<int>( group1.size() ); ++j )
            {
                for ( int k = 0; k < static_cast<int>( group2.size() ); ++k )
                {
                    bool is_merged = _try_merge_two_implicants( group1[j], group2[k], thread_private_qm_table[i] );
                    is_at_least_one_pair_merged |= is_merged;
                    if ( is_merged )
                    {
                        thread_private_is_implicant_merged[i][j] = true;
                        thread_private_is_implicant_merged[i + 1][k] = true;
                    }
                }
            }
            #pragma omp critical ( implicant )
            {
                for ( int i = 0; i < static_cast<int>( new_qm_table.size() ); ++i )
                {
                    new_qm_table[i].insert( new_qm_table[i].end(), thread_private_qm_table[i].begin(), thread_private_qm_table[i].end() );
                }
            }
            #pragma omp critical ( table )
            {
                for ( int i = 0; i < static_cast<int>( is_implicant_merged.size() ); ++i )
                {
                    for ( int j = 0; j < static_cast<int>( is_implicant_merged[i].size() ); ++j )
                    {   
                        is_implicant_merged[i][j] = is_implicant_merged[i][j] || thread_private_is_implicant_merged[i][j];
                    }
                }
            }
        }
    }

    #pragma omp parallel
    {
        ImplicantSet thread_private_prime_implicant_set;
        #pragma omp for nowait
        for ( int i = 0; i < static_cast<int>( is_implicant_merged.size() ); ++i )
        {
            for ( int j = 0; j < static_cast<int>( is_implicant_merged[i].size() ); ++j )
            {
                if ( !is_implicant_merged[i][j] )
                {   
                    thread_private_prime_implicant_set.push_back( _qm_table[i][j] );
                }       
            }
        }
        #pragma omp critical
        {
            _prime_implicant_set.insert( _prime_implicant_set.end(), thread_private_prime_implicant_set.begin(), thread_private_prime_implicant_set.end() );
        }
    }

#else
    int max_threads = omp_get_max_threads();
    std::vector<QMTable> private_qm_table( max_threads );
    std::vector<std::vector<std::vector<bool> > > private_is_implicant_merged( max_threads );
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        private_qm_table[thread_id].resize( _qm_table.size() - 1 );
        private_is_implicant_merged[thread_id].resize( _qm_table.size() );
        for ( int group_id = 0; group_id < static_cast<int>( _qm_table.size() ); ++group_id )
        {
            private_is_implicant_merged[thread_id][group_id].resize( _qm_table[group_id].size(), false );
        }
    }

    // try to merge all pairs of implicants between two adjacent group
    #pragma omp parallel
    {
        #pragma omp single
        {
            int group_pairs_count = static_cast<int>( _qm_table.size() ) - 1;
            for ( int group_id = 0; group_id < group_pairs_count; ++group_id )
            {
                const auto& group1 = _qm_table[group_id];
                const auto& group2 = _qm_table[group_id + 1];
                for ( int i = 0; i < static_cast<int>( group1.size() ); ++i )
                {
                    for ( int j = 0; j < static_cast<int>( group2.size() ); ++j )
                    {
                        #pragma omp task firstprivate( i, j, group_id ) shared( group1, group2, private_qm_table, private_is_implicant_merged )
                        {
                            int thread_id = omp_get_thread_num();
                            bool is_merged = _try_merge_two_implicants( group1[i], group2[j], private_qm_table[thread_id][group_id] );
                            if ( is_merged )
                            {
                                private_is_implicant_merged[thread_id][group_id][i] = true;
                                private_is_implicant_merged[thread_id][group_id + 1][j] = true;
                            }
                        }
                    }
                }
            }
        }
    }

    // integrating thread private data to global
    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        for ( int i = 0; i < static_cast<int>( is_implicant_merged.size() ); ++i )
        {
            for ( int j = 0; j < static_cast<int>( is_implicant_merged[i].size() ); ++j )
            {
                #pragma omp critical( bool_or )
                {
                    is_implicant_merged[i][j] = is_implicant_merged[i][j] || private_is_implicant_merged[thread_id][i][j];
                }
            }
        }
        for ( int group_id = 0; group_id < static_cast<int>( new_qm_table.size() ); ++group_id )
        {
            #pragma omp critical( vector_append )
            {
                new_qm_table[group_id].insert( new_qm_table[group_id].end(), private_qm_table[thread_id][group_id].begin(), private_qm_table[thread_id][group_id].end() );
            }
        }
    }

    // restoring non-merged implicants to  prime implicants set
    bool is_at_least_one_pair_merged = false;
    #pragma omp parallel
    {
        ImplicantSet private_prime_implicant_set;
        #pragma omp for reduction( | : is_at_least_one_pair_merged ) nowait
        for ( int group_id = 0; group_id < static_cast<int>( is_implicant_merged.size() ); ++group_id )
        {
            for ( int implicant_id = 0; implicant_id < static_cast<int>( is_implicant_merged[group_id].size() ); ++implicant_id )
            {   
                is_at_least_one_pair_merged |= is_implicant_merged[group_id][implicant_id];
                if ( !is_implicant_merged[group_id][implicant_id] )
                {   
                    private_prime_implicant_set.push_back( _qm_table[group_id][implicant_id] );
                }
            }
        }
        #pragma omp critical ( collect_prime_implicants )
        {
            _prime_implicant_set.insert( _prime_implicant_set.end(), private_prime_implicant_set.begin(), private_prime_implicant_set.end() );
        }
    }
#endif

    _qm_table_ptr = std::move( new_qm_table_ptr );
    return is_at_least_one_pair_merged;
}

bool QuineMcCluskey::_try_merge_two_implicants( const Implicant& lhs, const Implicant& rhs, ImplicantSet& new_implicant_set ) const
{
    std::vector<int> different_indices;
    for ( int i = 0; i < static_cast<int>( lhs.booleans.size() ); ++i )
    {   
        if ( lhs.booleans[i] != rhs.booleans[i] )
        {
            different_indices.push_back( i );
        }
    }
    if ( different_indices.size() != 1 )
    {
        return false;
    }
    Implicant merged_implicant;
    merged_implicant.set_size( static_cast<int>( lhs.booleans.size() ) );
    for ( int i = 0; i < static_cast<int>( lhs.booleans.size() ); ++i )
    {
        merged_implicant.set_value( i, lhs.booleans[i] );
    }
    merged_implicant.set_value( different_indices[0], Implicant::DONT_CARE );
#ifdef VER1
    #pragma omp critical
#endif
    new_implicant_set.push_back( merged_implicant );
    return true;
}

void QuineMcCluskey::_remove_redundant_implicants()
{
    #pragma omp parallel for
    for ( int group_id = 0; group_id < static_cast<int>( _qm_table_ptr->size() ); ++group_id )
    {
        auto& _qm_table = *_qm_table_ptr;
        std::sort( _qm_table[group_id].begin(), _qm_table[group_id].end() );
        _qm_table[group_id].erase( std::unique( _qm_table[group_id].begin(), _qm_table[group_id].end() ), _qm_table[group_id].end() );
    }
}

bool QuineMcCluskey::_check_prime_implicant_cover_input( const Implicant& prime_implicant, const Implicant& input_implicant ) const
{
    assert( prime_implicant.get_size() == input_implicant.get_size() );
    int size = prime_implicant.get_size();
    for ( int i = 0; i < size; ++i )
    {
        bool is_prime_dont_care = prime_implicant.get_value( i ) == Implicant::DONT_CARE;
        bool is_equal = prime_implicant.get_value( i ) == input_implicant.get_value( i );
        if ( !is_prime_dont_care && !is_equal )
        {
            return false;
        }
    }
    return true;
}

void QuineMcCluskey::_construct_prime_implicant_table( std::vector<std::vector<int> >& prime_implicant_table ) const
{
    prime_implicant_table.resize( 0 );
    prime_implicant_table.resize( _input_implicant_set.size() );
    
    #pragma omp parallel for collapse ( 2 )
    for ( int prime_id = 0; prime_id < static_cast<int>( _prime_implicant_set.size() ); ++prime_id )
    {
        for ( int input_id = 0; input_id < static_cast<int>( _input_implicant_set.size() ); ++input_id )
        {   
            const Implicant& prime_implicant = _prime_implicant_set[prime_id];
            const Implicant& input_implicant = _input_implicant_set[input_id];
            bool is_input_covered = _check_prime_implicant_cover_input( prime_implicant, input_implicant );
            if ( is_input_covered )
            {   
                #pragma omp critical
                {
                    prime_implicant_table[input_id].push_back( prime_id );
                }
            }
        }
    }
}

void QuineMcCluskey::get_petricks_initial_function( std::vector<std::vector<int> >& initial_function, std::vector<int>& costs ) const
{   
    // std::cout << "QM Prime implicants:" << std::endl;
    // for ( int i = 0; i < static_cast<int>( _prime_implicant_set.size() ); ++i )
    // {
    //     std::cout << _prime_implicant_set[i] << std::endl;
    // }

    // clear input container
    initial_function.resize( 0 );
    costs.resize( 0 );
    
    // construct prime implicant chart/table
    std::vector<std::vector<int> >& prime_implicant_table = initial_function;
    _construct_prime_implicant_table( prime_implicant_table );
    // _report_petricks_initial_function( initial_function );

    // output cost
    costs.resize( _prime_implicant_set.size() );
    #pragma omp parallel for
    for ( int i = 0; i < static_cast<int>( _prime_implicant_set.size() ); ++i )
    {
        costs[i] = _prime_implicant_set[i].get_cost();   
    }
}

void QuineMcCluskey::get_prime_implicant_strings( const std::vector<int>& selected_prime_implicant_ids, std::vector<std::string>& prime_implicant_strings ) const
{
    prime_implicant_strings.resize( selected_prime_implicant_ids.size() );
    #pragma omp parallel for
    for ( int i = 0; i < static_cast<int>( selected_prime_implicant_ids.size() ); ++i )
    {
        int id = selected_prime_implicant_ids[i];
        int variable_count = static_cast<int>( _prime_implicant_set[id].booleans.size() );
        std::string& str_implicant = prime_implicant_strings[i];
        str_implicant.resize( variable_count );
        for ( int j = 0; j < variable_count; ++j )
        {
            Implicant::Boolean bit_value = _prime_implicant_set[id].booleans[j];
            if ( bit_value == Implicant::ZERO )
            {
                str_implicant[j] = '0';
            }
            else if ( bit_value == Implicant::ONE )
            {
                str_implicant[j] = '1';
            }
            else
            {
                str_implicant[j] = '-';
            }
        }
    }
}

void QuineMcCluskey::_report_qm_table() const
{
    std::cout << "QM implicant groups : " << _qm_table_ptr->size() << std::endl;
    for ( int i = 0; i < static_cast<int>( _qm_table_ptr->size() ); ++i )
    {
        if ( ( *_qm_table_ptr )[i].size() == 0 )
        {
            continue;
        }
        std::cout << "    implicant count in group " << i << " : " << ( *_qm_table_ptr )[i].size() << std::endl;
    }
}

void QuineMcCluskey::_collect_prime_implicants_from_qm_table()
{
    const auto& _qm_table = *_qm_table_ptr;
    for ( int group_id = 0; group_id < static_cast<int>( _qm_table.size() ); ++group_id )
    {
        for ( int implicant_id = 0; implicant_id < static_cast<int>( _qm_table[group_id].size() ); ++implicant_id )
        {
            _prime_implicant_set.push_back( _qm_table[group_id][implicant_id] );
        }
    }
    _qm_table_ptr.reset();
}

void QuineMcCluskey::_report_petricks_initial_function( const std::vector<std::vector<int> >& initial_function ) const
{
    std::cout << "Product of sums :" << std::endl;
    for ( int i = 0; i < static_cast<int>( initial_function.size() ); ++i )
    {
        std::cout << "    Variable " << i << " :";
        for ( int j = 0; j < static_cast<int>( initial_function[i].size() ); ++j )
        {
            std::cout << " " << initial_function[i][j];
        }
        std::cout << std::endl;
    }
}