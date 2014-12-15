#include "cache_block_info.h"
#include "pr_l1_cache_block_info.h"
#include "pr_l2_cache_block_info.h"
#include "shared_cache_block_info.h"
#include "log.h"

const char* CacheBlockInfo::option_names[] =
{
   "prefetch",
   "warmup",
};

const char* CacheBlockInfo::getOptionName(option_t option)
{
   static_assert(CacheBlockInfo::NUM_OPTIONS == sizeof(CacheBlockInfo::option_names) / sizeof(char*), "Not enough values in CacheBlockInfo::option_names");

   if (option < NUM_OPTIONS)
      return option_names[option];
   else
      return "invalid";
}


CacheBlockInfo::CacheBlockInfo(IntPtr tag, CacheState::cstate_t cstate, UInt64 options):
   m_tag(tag),
   m_cstate(cstate),
   m_owner(0),
   m_used(0),
   m_options(options)
{
    m_faulty = false;  		// ABM : reset
}

CacheBlockInfo::~CacheBlockInfo()
{}

CacheBlockInfo*
CacheBlockInfo::create(CacheBase::cache_t cache_type)
{
   switch (cache_type)
   {
      case CacheBase::PR_L1_CACHE:
         return new PrL1CacheBlockInfo();

      case CacheBase::PR_L2_CACHE:
         return new PrL2CacheBlockInfo();

      case CacheBase::SHARED_CACHE:
         return new SharedCacheBlockInfo();

      default:
         LOG_PRINT_ERROR("Unrecognized cache type (%u)", cache_type);
         return NULL;
   }
}

void
CacheBlockInfo::invalidate()
{
   m_tag = ~0;
   m_cstate = CacheState::INVALID;
}

void
CacheBlockInfo::clone(CacheBlockInfo* cache_block_info)
{
   m_tag = cache_block_info->getTag();
   m_cstate = cache_block_info->getCState();
   m_owner = cache_block_info->m_owner;
   m_used = cache_block_info->m_used;
   m_options = cache_block_info->m_options;
}

bool
CacheBlockInfo::updateUsage(UInt32 offset, UInt32 size)
{
   UInt64 first = offset >> BitsUsedOffset,
          last  = (offset + size - 1) >> BitsUsedOffset,
          first_mask = (1ull << first) - 1,
          last_mask = (1ull << (last + 1)) - 1,
          usage_mask = last_mask & ~first_mask;

   return updateUsage(usage_mask);
}

bool
CacheBlockInfo::updateUsage(BitsUsedType used)
{
   bool new_bits_set = used & ~m_used; // Are we setting any bits that were previously unset?
   m_used |= used;                     // Update usage mask
   return new_bits_set;
}

/** ABM :
	  * Generates faults for this block. This should only be called before execution begins.
	  * Returns the faultMap code generated for this block.

int
CacheBlockInfo::generateFault(UInt64 ber, UInt32 block_size)
{
	bool tmp_isFaulty;
	UInt64 tmp_rnd;

    //compute the new faults on any so-far non-faulty cells
	tmp_isFaulty = false;
    unsigned long outcome = 0;
    for (UInt32 j = 0; j < block_size*8; j++) {
    	tmp_rnd = rng_next(m_random);
    	outcome = tmp_rnd % ber;
        if (outcome == 1) {
                //e.g. if bitFaultRates[i] == 1e12, this should generate a random number between 0 and (1e12), inclusive.
                //the outcome is then true if the result was exactly one fixed value, say, 0.
        	tmp_isFaulty = true;
        	LOG_PRINT_ERROR("Random number is: %ld, %d, %d\n", tmp_rnd, ber, tmp_rnd % ber);
        	break;
       }
    }
    isFaulty = tmp_isFaulty;

    return 0;
} */
