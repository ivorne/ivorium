macro( get_target_property_recursive var target property )
	
	# process dependencies
	get_property( TGTS_DEFINED TARGET ${target} PROPERTY LINK_LIBRARIES SET )
	if( TGTS_DEFINED )
		get_property( TGTS TARGET ${target} PROPERTY LINK_LIBRARIES )
		foreach( TGT IN LISTS TGTS )
			
			if( TARGET ${TGT} )
				get_property( TYPE TARGET ${TGT} PROPERTY TYPE )
				if( NOT TYPE STREQUAL INTERFACE_LIBRARY )
					get_target_property_recursive( ${var} ${TGT} ${property} )
				endif()
			endif()
			
		endforeach()
	endif()
	
	# add resource dir from target
	get_property( RESDIR_DEFINED TARGET ${target} PROPERTY ${property} SET )
	if( RESDIR_DEFINED )
		get_property( RESDIR TARGET ${target} PROPERTY ${property} )
		if( NOT RESDIR IN_LIST ${var} )
			set( ${var} ${${var}} ${RESDIR} )
		endif()
	endif()
	
endmacro()
