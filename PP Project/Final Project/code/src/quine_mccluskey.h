#ifndef QUINE_MCCLUSKEY_H
#define QUINE_MCCLUSKEY_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <memory>

class QuineMcCluskey
{

private:

    struct Implicant
    {

    public:

        enum Boolean
        {
            ZERO = 0,
            ONE = 1,
            DONT_CARE = 2
        };
        Implicant()
            : booleans()
        {}
        Implicant( const std::string& input_string );
        int get_ones_count() const;
        int get_dont_cares_count() const;
        int get_cost() const;
        int get_size() const { return static_cast<int>( booleans.size() ); }
        Boolean get_value( int index ) const { return booleans[index]; }
        void set_size( int size );
        void set_value( int index, Boolean value );
        void get_on_set_values( std::vector<int>& on_set_values ) const;

    public:

        std::vector<Boolean> booleans;

    };

public:

    friend std::ostream& operator<<( std::ostream& os, const Implicant& implicant );
    friend bool operator<( const Implicant& lhs, const Implicant& rhs );
    friend bool operator==( const Implicant& lhs, const Implicant& rhs );
    using ImplicantSet = std::vector<Implicant>;
    using QMTable = std::vector<ImplicantSet>;

public:

    QuineMcCluskey();
    void run( const std::vector<std::string>& input_implicants );
    void get_petricks_initial_function( std::vector<std::vector<int> >& initial_function, std::vector<int>& costs ) const;
    void get_prime_implicant_strings( const std::vector<int>& selected_prime_implicant_ids, std::vector<std::string>& prime_implicant_strings ) const;

private:
    
    void _report_qm_table() const;
    void _report_petricks_initial_function( const std::vector<std::vector<int> >& initial_function ) const;
    bool _initialize_qm_table( const std::vector<std::string>& input_implicants );
    void _check_variable_count_consistent( const std::vector<std::string>& input_implicants ) const;
    bool _try_merge_all_pairs_of_implicants();
    void _remove_redundant_implicants();
    bool _try_merge_two_implicants( const Implicant& lhs, const Implicant& rhs, ImplicantSet& new_implicant_set ) const;
    void _collect_prime_implicants_from_qm_table();
    void _construct_prime_implicant_table( std::vector<std::vector<int> >& prime_implcant_chart ) const;
    bool _check_prime_implicant_cover_input( const Implicant& prime_implicant, const Implicant& input_implicant ) const;

private:

    std::unique_ptr<QMTable> _qm_table_ptr;
    ImplicantSet _input_implicant_set;
    ImplicantSet _prime_implicant_set;

};

#endif // QUINE_MCCLUSKEY_H